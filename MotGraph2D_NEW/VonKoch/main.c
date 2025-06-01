#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include "utils_SDL.h"
#include "mot.h"

void displayAll(SDL_Surface* window, listO* O) {
	obj* o;
	listP* P;
	pointf* p;
	int offScreen;
	int w = window->w;
	int h = window->h;

	seekCollision(O);

	while (O) {
		o = O->o;
		o->v->y += 0.0001;
		if (!o->isStatic) {
			P = o->points;

			offScreen = 1;
			while (P) {
				p = P->p;
				movePoint(o, p);
				displayVector(window, o, *p, 200);

				offScreen = offScreen && (p->x < 0.f || p->x > w || p->y < 0.f || p->y > h);
				
				P = P->next;
			}
			movePoint(o, &(o->pRot));

			if (offScreen) {
				//printf("OFFSCREEN !\n");
				//freeObj(o);
				//*O = *(O->next);
				//continue;
			}
		}
		colorObj(window, o->points, o->nbPoints, o->color);
		O = O->next;
	}
}

int main(int argc, char **argv)
{

	SDL_Surface * window;
	int quit = 0;
	int pause = 0;
	SDL_Event e;
	Uint8 mouseButton;
	point mousePos;

	obj* o = NULL;
	listO* O = NULL;

	listP* P1 = NULL;
	addPoint(&P1, createPoint(50, 400));
	addPoint(&P1, createPoint(590, 400));
	addPoint(&P1, createPoint(320, 450));
	obj* o1 = createObj(P1, 200, 1, 1);
	addObj(&O, o1);

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
				if (pause == 0) o = createObj(createList(createPoint(mousePos.x, mousePos.y)), SDL_MapRGB(window->format, 160, 190 * (e.button.button == SDL_BUTTON_RIGHT), 220), 0, (e.button.button == SDL_BUTTON_RIGHT));
				else {
					addPoint(&o->points, createPoint(mousePos.x, mousePos.y));
					o->nbPoints++;
				}

				SDL_FillRect(window, &(window->clip_rect), SDL_MapRGB(window->format, 0, 0, 0));
				colorObj(window, o->points, o->nbPoints, o->color);
				displayAll(window, O);
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

	freeListO(O);

	return EXIT_SUCCESS;
}