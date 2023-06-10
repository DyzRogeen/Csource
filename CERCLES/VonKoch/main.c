#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include "circle.h"
#include "frac.h"
#include "utils_SDL.h"

/*
E - Effacer écran
A - Ajouter cercle
Q - Ajout bocou
X - Suppr cercle
Z - Afficher dernier cercle
*/

int main(int argc, char **argv)
{
	cercle* circle = (cercle*)calloc(1, sizeof(cercle));

	float r, w, p;

	int iter = 0;
	int mode = 0;
	int color = 0;

	point a, b;
	SDL_Surface * window;
	int iterations = 0;
	int quit = 0;
	int figure = 0;
	int last_circle = 0;
	enum { dessin1, dessin2, rendu } state = dessin1;
	SDL_Event e;
	Uint8 mouseButton;
	point mousePos;

	// Initialisation
	if (SDL_Init(SDL_INIT_VIDEO) < 0) {
		fprintf(stderr, "ERREUR - SDL_Init\n>>> %s\n", SDL_GetError());
		exit(EXIT_FAILURE);
	}

	// Permet de quitter proprement par la suite (revenir à la résolution initiale de l'écran avec le bon format vidéo)
	atexit(SDL_Quit);
	// Initialise le mode vidéo de la fenêtre
	window = SDL_SetVideoMode(1280, 700, 32, SDL_HWSURFACE | SDL_DOUBLEBUF);

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
				if (e.key.keysym.sym == SDLK_UP)
					iterations++;
				if (e.key.keysym.sym == SDLK_DOWN)
				{
					iterations--;
					iterations = iterations >= 0 ? iterations : 0;
				}
				if (e.key.keysym.sym == SDLK_f)
				{
					if (figure == 1)
						figure = 0;
					else
						figure = 1;
				}
				if (e.key.keysym.sym == SDLK_t)
				{
					if (figure == 2)
						figure = 0;
					else
						figure = 2;
				}
				if (e.key.keysym.sym == SDLK_s)
				{
					if (figure == 3)
						figure = 0;
					else
						figure = 3;
				}
				if (e.key.keysym.sym == SDLK_w) {
					printf("Lastcircle : %d\n", !last_circle);
					last_circle = last_circle == 0 ? 1 : 0;
					SDL_FillRect(window, &(window->clip_rect), SDL_MapRGB(window->format, 0, 0, 0));
				}
				if (e.key.keysym.sym == SDLK_q) {
					printf("ADDCIRCLE : Selec mode :\n 0 : cercle entier,\n 1 : (cos) abscisse seulement,\n 2 : (sin) ordonnee seulement.\n> ");
					scanf_s("%d", &mode);
					printf("ADDCIRCLE : (Format : radius pulsation phase) : ");
					scanf_s("%f %f %f", &r, &w, &p);
					addCircle(&circle, r, w, p, mode);
				}
				if (e.key.keysym.sym == SDLK_x) {
					removeCircle(&circle);
					SDL_FillRect(window, &(window->clip_rect), SDL_MapRGB(window->format, 0, 0, 0));
				}
				if (e.key.keysym.sym == SDLK_a) {
					printf("STRAIGHT : Nombre d'iterations : ");
					scanf_s("%d", &iter, 1);
					straight(&circle, 1, iter + 1);
				}
				if (e.key.keysym.sym == SDLK_c)
					color = color == 0 ? 1 : 0;
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
					state = rendu;
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
			if (e.key.keysym.sym == SDLK_e)
				SDL_FillRect(window, &(window->clip_rect), SDL_MapRGB(window->format, 0, 0, 0));

			switch (figure)
			{
			case 1:
				flocKoch(window, iterations, a, b); break;
			case 2:
				sierpangle(window, iterations, a, b); break;
			case 3:
				sierponge(window, iterations, a, b); break;
			default:
				vonKoch(window, iterations, a, b);
			}

			affCircle(window, circle, 640, 350, last_circle, color);
			//Uint32 coul = SDL_MapRGB(window->format, 255, 255, 255);
			//_SDL_SetPixel(window, 320 + 200 * cos(2 * PI * SDL_GetTicks() / 1000), 240 + 200 * sin(2 * PI * SDL_GetTicks() / 1000), coul);

			//_SDL_DrawLine(window, 320, 240, 320 + 200*cos(2*PI*SDL_GetTicks() / 10000), 240 + 200 * sin(2 * PI * SDL_GetTicks() / 10000), 255, 255, 255);
			// Met à jour le buffer de la fenêtre
			SDL_UpdateRect(window, 0, 0, 0, 0);
			// Flip le buffer pour l'envoyer vers l'écran (la fenêtre est configurée en double buffer)
			SDL_Flip(window);
		}

	}

	freeCircle(&circle);

	return EXIT_SUCCESS;
}