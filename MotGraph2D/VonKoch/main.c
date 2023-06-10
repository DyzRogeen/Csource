#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include "utils_SDL.h"
#include "mot.h"

#define PI	3.14159265359

float norm(point v)
{
	return sqrtf(v.m_x * v.m_x + v.m_y * v.m_y);
}

int main(int argc, char **argv)
{
	point a, b, c;
	SDL_Surface * window;
	int iterations = 0;
	int quit = 0;
	enum { dessin1, dessin2, rendu } state = dessin1;
	SDL_Event e;
	Uint8 mouseButton;
	point mousePos;

	list* l = NULL;
	float dx = 0;
	float dy = 0;
	dep d = 3;
	int i = 0;
	int stop = 0;

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

	while (!quit)
	{
		while (SDL_PollEvent(&e))
		{
			switch (e.type)
			{
			case SDL_QUIT:
				quit = 1;
				break;
			case SDL_KEYUP:
				if ((e.key.keysym.sym == SDLK_LEFT || e.key.keysym.sym == SDLK_RIGHT))
					stop = 1;
				break;
			case SDL_KEYDOWN:
				if (e.key.keysym.sym == SDLK_ESCAPE)
					quit = 1;
				if (e.key.keysym.sym == SDLK_a) {
					popList(&l);
					printList(l);
				}
				if (e.key.keysym.sym == SDLK_LEFT) {
					d = LEFT;
					stop = 0;
				}
				if (e.key.keysym.sym == SDLK_RIGHT) {
					d = RIGHT;
					stop = 0;
				}
				if (e.key.keysym.sym == SDLK_UP && dy == 0) dy -= 10;
				break;
			case SDL_MOUSEBUTTONDOWN:
				mouseButton = SDL_GetMouseState(&mousePos.m_x, &mousePos.m_y);

				switch (state)
				{
				case dessin1:
					a.m_x = mousePos.m_x;
					a.m_y = mousePos.m_y;
					state = dessin2;
					break;
				case dessin2:
					b.m_x = mousePos.m_x;
					b.m_y = mousePos.m_y;

					if (b.m_x < a.m_x) {
						b.m_x = a.m_x;
						a.m_x = mousePos.m_x;
					}
					if (b.m_y < a.m_y) {
						b.m_y = a.m_y;
						a.m_y = mousePos.m_y;
					}
					state = rendu;

					bloc* rect = NULL;
					if (e.button.button == SDL_BUTTON_RIGHT) rect = createBloc(a, b, STATIC);
					if (e.button.button == SDL_BUTTON_LEFT) rect = createBloc(a, b, DYNAMIC);
					insertList(&l, rect);
					printList(l);

					dx = 0.0;
					dy = 0.0;
					d = DOWN;

					break;
				case rendu:
				default:
					a.m_x = mousePos.m_x;
					a.m_y = mousePos.m_y;
					state = dessin2;
					iterations = 0;
					break;
				}

				break;
			}
		}

		if (state == rendu)
		{

			// On vide la fenêtre (on la remplit en noir)
			SDL_FillRect(window, &(window->clip_rect), SDL_MapRGB(window->format, 0, 0, 0));

			affAll(l, window);

			if (i == 15) {

				if (dx < 6.0 && d == RIGHT && dy == 0) dx += 0.25;
				if (dx > -6.0 && d == LEFT && dy == 0) dx -= 0.25;
				if (stop == 1 && dy == 0) {
					if (dx > 0) dx -= 0.5;
					if (dx < 0) dx += 0.5;
				}

				dy += 0.981;

				//printf("Dir %d | dx = %d | dy = %d\n", d, (int)dx, (int)dy);

				moveBloc(l, d, &dx, &dy);
				moveBloc(l, DOWN, &dx, &dy);

				i = 0;
			}

			// Met à jour le buffer de la fenêtre
			SDL_UpdateRect(window, 0, 0, 0, 0);
			// Flip le buffer pour l'envoyer vers l'écran (la fenêtre est configurée en double buffer)
			SDL_Flip(window);

			i++;
		}

	}

	freeList(l);

	return EXIT_SUCCESS;
}