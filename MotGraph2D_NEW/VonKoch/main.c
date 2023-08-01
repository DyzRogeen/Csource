#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include "utils_SDL.h"
#include "mot.h"

void displayAll(SDL_Surface* window, listO* O) {
	obj* o;
	for (int i = 0; O; i++) {
		o = O->o;
		if (!o->isStatic) {
			moveObj(o);
			o->velocity.y += 0.01;
		}

		colorObj(window, o->points, o->nbPoints, o->color);
		O = O->next;
	}
}

int main(int argc, char **argv)
{
	point* a;
	SDL_Surface * window;
	int quit = 0;
	int pause = 0;
	SDL_Event e;
	Uint8 mouseButton;
	point mousePos;

	obj* o = NULL;
	listO* O = NULL;

	int nbPoints = 1;

	// DIM
	int w = 640;
	int h = 480;

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
		fprintf(stderr, "ERREUR - impossible de passer en : %dx%dx%d\n>>> %s\n", w, h, 32, SDL_GetError());
		exit(EXIT_FAILURE);
	}

	a = createPoint(w/2 + 100,	h/2 + 100);

	listP* P = createList(a);

	// On met le titre sur la fenêtre
	SDL_WM_SetCaption("2D Engine", NULL);

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
				if (pause == 1 && e.key.keysym.sym == SDLK_LCTRL) {
					addObj(&O, o);
					pause = 0;
				}
				break;
			case SDL_MOUSEBUTTONDOWN:
				mouseButton = SDL_GetMouseState(&mousePos.x, &mousePos.y);
				if (pause == 0) o = createObj(createList(createPoint(mousePos.x, mousePos.y)), SDL_MapRGB(window->format, 160, 130, 220), 0, (e.button.button == SDL_BUTTON_RIGHT));
				else {
					addPoint(&o->points, createPoint(mousePos.x, mousePos.y));
					o->nbPoints++;
				}

				colorObj(window, o->points, o->nbPoints, o->color);
				SDL_UpdateRect(window, 0, 0, 0, 0);
				SDL_Flip(window);

				pause = 1;
				break;
			}
		}

		if (!pause)
		{
			// On vide la fenêtre (on la remplit en noir)
			SDL_FillRect(window, &(window->clip_rect), SDL_MapRGB(window->format, 0, 0, 0));

			displayAll(window, O);
			// Met à jour le buffer de la fenêtre
			SDL_UpdateRect(window, 0, 0, 0, 0);
			// Flip le buffer pour l'envoyer vers l'écran (la fenêtre est configurée en double buffer)
			SDL_Flip(window);
		}

	}

	return EXIT_SUCCESS;
}