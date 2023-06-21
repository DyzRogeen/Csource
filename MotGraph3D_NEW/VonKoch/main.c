#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include "utils_SDL.h"
#include "mot.h"

void updateScreen(SDL_Surface * window, obj o, cam C) {

	// On vide la fenêtre (on la remplit en noir)
	SDL_FillRect(window, &(window->clip_rect), SDL_MapRGB(window->format, 0, 0, 0));

	displayObj(window, o, C);

	// Met à jour le buffer de la fenêtre
	SDL_UpdateRect(window, 0, 0, 0, 0);
	// Flip le buffer pour l'envoyer vers l'écran (la fenêtre est configurée en double buffer)
	SDL_Flip(window);
}

int main(int argc, char** argv)
{
	SDL_Surface* window;
	SDL_Event e;

	int quit = 0;
	int dirx = 0;
	int diry = 0;
	point mPos, premPos;
	SDL_GetMouseState(&premPos.x, &premPos.y);
	float speed = 0.05;
	float jsp = 1;

	point3 n;
	cam C = initCam(setPoint(0, 300, 0), setPoint(0.1, 0, 0));

	point3* a = createPoint(50, 400, -100);
	point3* b = createPoint(50, 400, 100);
	point3* c = createPoint(50, 200, 100);
	point3* d = createPoint(50, 200, -100);

	point3* a1 = createPoint(250, 400, -100);
	point3* b1 = createPoint(250, 400, 100);
	point3* c1 = createPoint(250, 200, 100);
	point3* d1 = createPoint(250, 200, -100);

	edge* E[12] = {
		createEdge(a,b),
		createEdge(b,c),
		createEdge(c,d),
		createEdge(d,a),

		createEdge(a1,b1),
		createEdge(b1,c1),
		createEdge(c1,d1),
		createEdge(d1,a1),

		createEdge(a,a1),
		createEdge(b,b1),
		createEdge(c,c1),
		createEdge(d,d1),
	};
	for (int i = 11; i > 0; i--) E[i]->next = E[i - 1];

	point3* P[8] = { a,b,c,d,a1,b1,c1,d1 };
	for (int i = 7; i > 0; i--) P[i]->next = P[i - 1];

	n.x = 1;
	n.y = 1;
	n.z = 1;

	face f[6];

	obj* o = createObj3D(f, E[11], P[7], 0, 12, 8);

	// Initialisation
	if (SDL_Init(SDL_INIT_VIDEO) < 0) {
		fprintf(stderr, "ERREUR - SDL_Init\n>>> %s\n", SDL_GetError());
		exit(EXIT_FAILURE);
	}

	// Permet de quitter proprement par la suite (revenir à la résolution initiale de l'écran avec le bon format vidéo)
	atexit(SDL_Quit);
	// Initialise le mode vidéo de la fenêtre
	window = SDL_SetVideoMode(640, 480, 32, SDL_HWSURFACE | SDL_DOUBLEBUF);
	// Si on n'a pas réussi à créer la fenêtre on retourne une erreur
	if (window == NULL) {
		fprintf(stderr, "ERREUR - impossible de passer en : %dx%dx%d\n>>> %s\n", 640, 480, 32, SDL_GetError());
		exit(EXIT_FAILURE);
	}

	// On met le titre sur la fenêtre
	SDL_WM_SetCaption("VonKoch", NULL);

	updateScreen(window, *o, C);

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
				if (e.key.keysym.sym == SDLK_w) dirx = 1;
				if (e.key.keysym.sym == SDLK_s) dirx = -1;
				if (e.key.keysym.sym == SDLK_a) diry = -1;
				if (e.key.keysym.sym == SDLK_d) diry = 1;
				if (e.key.keysym.sym == SDLK_r) C = initCam(setPoint(0,300,0),setPoint(jsp,0,0));
				if (e.key.keysym.sym == SDLK_e) jsp /= 10;
				if (e.key.keysym.sym == SDLK_q) jsp *= 10;
					
				break;
			case SDL_KEYUP:
				if (e.key.keysym.sym == SDLK_w || e.key.keysym.sym == SDLK_s) dirx = 0;
				if (e.key.keysym.sym == SDLK_a || e.key.keysym.sym == SDLK_d) diry = 0;
			case SDL_MOUSEBUTTONDOWN:
				//mouseButton = SDL_GetMouseState(&mousePos.m_x, &mousePos.m_y);


				break;
			}
		}

		SDL_GetMouseState(&mPos.x, &mPos.y);
		if (mPos.x - premPos.x || mPos.y - premPos.y) {
			rotateCam(&C, (mPos.x - premPos.x) / 100.0, (mPos.y - premPos.y) / -100.0);
			updateScreen(window, *o, C);
		}

		if (dirx || diry) {
			moveCam(&C,setPoint((dirx * C.dir.x - diry * C.dir.z)*speed/jsp, 0, (dirx * C.dir.z + diry * C.dir.x)*speed/jsp));
			updateScreen(window, *o, C);
		}

		premPos.x = mPos.x;
		premPos.y = mPos.y;

	}

	return EXIT_SUCCESS;
}