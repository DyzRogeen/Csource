#define _CRT_SECURE_NO_WARNINGS 1
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include "utils_SDL.h"
#include "moteur3D.h"

int main(int argc, char **argv)
{
	pointS m;
	pointS mc;
	SDL_Surface * window;
	int quit = 0;
	SDL_Event e;
	Uint8 mouseButton;
	pointS mousePos;
	float x, y, z;
	int id1, id2;
	int i = 0;
	int dirx = 0;
	int diry = 0;

	cam c = initCam(0, 0, 0, 0, 0, 1);
	obj** Head = (obj**)calloc(1, sizeof(obj*));
	obj* o1 = NULL;
	addPoint(&o1, createPoint(200, 100, 100));
	addPoint(&o1, createPoint(200, -100, 100));
	addPoint(&o1, createPoint(200, 100, -100));
	addPoint(&o1, createPoint(200, -100, -100));
	addPoint(&o1, createPoint(300, 100, 100));
	addPoint(&o1, createPoint(300, -100, 100));
	addPoint(&o1, createPoint(300, 100, -100));
	addPoint(&o1, createPoint(300, -100, -100));
	addNeighbor(o1, 0, 1);
	addNeighbor(o1, 0, 2);
	addNeighbor(o1, 3, 1);
	addNeighbor(o1, 3, 2);
	addNeighbor(o1, 4, 5);
	addNeighbor(o1, 4, 6);
	addNeighbor(o1, 7, 5);
	addNeighbor(o1, 7, 6);
	addNeighbor(o1, 0, 4);
	addNeighbor(o1, 1, 5);
	addNeighbor(o1, 2, 6);
	addNeighbor(o1, 3, 7);
	addObject(Head, o1);
	affObject(o1);
	screen s = getScreen(c);
	
	// Initialisation
	if (SDL_Init(SDL_INIT_VIDEO) < 0) {
		fprintf(stderr, "ERREUR - SDL_Init\n>>> %s\n", SDL_GetError());
		exit(EXIT_FAILURE);
	}
	
	// Permet de quitter proprement par la suite (revenir à la résolution initiale de l'écran avec le bon format vidéo)
	atexit(SDL_Quit);
	// Initialise le mode vidéo de la fenêtre
	window = SDL_SetVideoMode(640, 480, 32, SDL_HWSURFACE | SDL_DOUBLEBUF);
	SDL_WarpMouse(320, 240);
	SDL_GetMouseState(&mc.m_x, &mc.m_y);
	SDL_ShowCursor(0);
	
	// Si on n'a pas réussi à créer la fenêtre on retourne une erreur
	if (window == NULL) {
		fprintf(stderr, "ERREUR - impossible de passer en : %dx%dx%d\n>>> %s\n", 640, 480, 32, SDL_GetError());
		exit(EXIT_FAILURE);
	}
	
	// On met le titre sur la fenêtre
	SDL_WM_SetCaption("VonKoch", NULL);
	
	while (!quit)
	{
		while (SDL_PollEvent(&e))
		{
			switch (e.type)
			{
			case SDL_QUIT:
				quit = 1;
				break;
			case SDL_KEYDOWN:
				if (e.key.keysym.sym == SDLK_ESCAPE)
					quit = 1;
				if (e.key.keysym.sym == SDLK_q)
				{
					printf("INSERT POINT format :'X Y Z'\n");
					scanf("%f %f %f", &x, &y, &z);
					addPoint(&o1, createPoint(x, y, z));
					affObject(o1);
				}
				if (e.key.keysym.sym == SDLK_e)
				{
					printf("ADD NEIGHBOR format :'id1 id2'\n");
					scanf("%d %d", &id1, &id2);
					addNeighbor(o1, id1, id2);
				}
				if (e.key.keysym.sym == SDLK_r) c = initCam(0, 0, 0, 0, 0, 1);
				if (e.key.keysym.sym == SDLK_x) addCam(&c, 0, 0, 0, PI / 2, 0, 0);
				if (e.key.keysym.sym == SDLK_w) dirx = 1;
				if (e.key.keysym.sym == SDLK_s) dirx = -1;
				if (e.key.keysym.sym == SDLK_a) diry = -1;
				if (e.key.keysym.sym == SDLK_d) diry = 1;
				break;
			case SDL_KEYUP:
				if (e.key.keysym.sym == SDLK_w || e.key.keysym.sym == SDLK_s) dirx = 0;
				if (e.key.keysym.sym == SDLK_a || e.key.keysym.sym == SDLK_d) diry = 0;
	
			}
			switch (e.button.button) {
			case SDL_BUTTON_WHEELUP:
				addCam(&c, 0, 0, 0, 0, 0, 0.5);
				break;
			case SDL_BUTTON_WHEELDOWN:
				addCam(&c, 0, 0, 0, 0, 0, -0.5);
				break;
			}
		}
	
		if (dirx != 0) {
			addCam(&c, dirx * 10 * cos(c.lon), dirx * 10 * sin(c.lon), 0, 0, 0, 0);
			//dirx = 0;
		}
		if (diry != 0) {
			addCam(&c, diry * 10 * sin(c.lon), diry * 10 * cos(c.lon), 0, 0, 0, 0);
			//diry = 0;
		}
		

		// On vide la fenêtre (on la remplit en noir)
		
		SDL_FillRect(window, &(window->clip_rect), SDL_MapRGB(window->format, 0, 0, 0));
		system("cls");
		
		SDL_GetMouseState(&m.m_x, &m.m_y);
		addCam(&c, 0, 0, 0, (m.m_x - mc.m_x) / 100.0, (m.m_y - mc.m_y) / 100.0, 0);
		printf("[lon = %f, lat = %f], %.0f\n", c.lon, c.lat, c.zoom);
		
		mc.m_x = m.m_x;
		mc.m_y = m.m_y;

		displayAll(window, c, *Head);

		// Met à jour le buffer de la fenêtre
		SDL_UpdateRect(window, 0, 0, 0, 0);
		// Flip le buffer pour l'envoyer vers l'écran (la fenêtre est configurée en double buffer)
		SDL_Flip(window);

	}

	return EXIT_SUCCESS;
}