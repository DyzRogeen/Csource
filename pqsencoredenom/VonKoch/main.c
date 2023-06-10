#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include "utils_SDL.h"
#include "mot.h"

#define PI	3.14159265359

/**
 *	@brief	Fonction permettant de calculer la norme d'un vecteur.
 *
 *	@param	v		Vecteur dont on souhaite calculer la norme.
 *	@return			norme du vecteur pass� en param�tre
 */

float norm(point v)
{
	return sqrtf(v.x * v.x + v.y * v.y);
}

int main(int argc, char **argv)
{
	point a, b, c;
	SDL_Surface * window;
	int iterations = 0;
	int quit = 0;
	int state = 0;
	SDL_Event e;
	Uint8 mouseButton;
	point mousePos;
	int flocon = 0;

	// Initialisation
	if (SDL_Init(SDL_INIT_VIDEO) < 0) {
		fprintf(stderr, "ERREUR - SDL_Init\n>>> %s\n", SDL_GetError());
		exit(EXIT_FAILURE);
	}

	// Permet de quitter proprement par la suite (revenir � la r�solution initiale de l'�cran avec le bon format vid�o)
	atexit(SDL_Quit);
	// Initialise le mode vid�o de la fen�tre
	window = SDL_SetVideoMode(640, 480, 32, SDL_HWSURFACE | SDL_DOUBLEBUF);

	// Si on n'a pas r�ussi � cr�er la fen�tre on retourne une erreur
	if (window == NULL) {
		fprintf(stderr, "ERREUR - impossible de passer en : %dx%dx%d\n>>> %s\n", 640, 480, 32, SDL_GetError());
		exit(EXIT_FAILURE);
	}

	// On met le titre sur la fen�tre
	SDL_WM_SetCaption("jsp", NULL);

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
				if (e.key.keysym.sym == SDLK_a)
					iterations++;
				if (e.key.keysym.sym == SDLK_q)
				{
					iterations--;
					iterations = iterations >= 0 ? iterations : 0;
				}
				if (e.key.keysym.sym == SDLK_f)															// L'appuie sur F active/d�sactive l'aspect "flocon"
					flocon = (flocon+1) % 2;
				break;
			case SDL_MOUSEBUTTONDOWN:
				mouseButton = SDL_GetMouseState(&mousePos.x, &mousePos.y);

				if (state == 0) {
					a.x = mousePos.x;
					a.y = mousePos.y;
					state = 1;
				}
				else {
					b.x = mousePos.x;
					b.y = mousePos.y;
					addNode(&l,createRect(a,b));
					state = 0;
				}

				break;
			}
		}

		if (state == 0)
		{
			// On vide la fen�tre (on la remplit en noir)
			SDL_FillRect(window, &(window->clip_rect), SDL_MapRGB(window->format, 0, 0, 0));
			displayRect(window, l);

			// Met � jour le buffer de la fen�tre
			SDL_UpdateRect(window, 0, 0, 0, 0);
			// Flip le buffer pour l'envoyer vers l'�cran (la fen�tre est configur�e en double buffer)
			SDL_Flip(window);
		}

	}

	freeList(l);

	return EXIT_SUCCESS;
}