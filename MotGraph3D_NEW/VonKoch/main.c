#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include "utils_SDL.h"
#include "mot.h"

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
	int flocon = 0;

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
				if (e.key.keysym.sym == SDLK_f)															// L'appuie sur F active/désactive l'aspect "flocon"
					flocon = (flocon+1) % 2;
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
					c.m_x = a.m_x + ((b.m_x - a.m_x) * cos(PI / 3.0f) - (b.m_y - a.m_y) * sin(PI / 3.0f));
					c.m_y = a.m_y + ((b.m_x - a.m_x) * sin(PI / 3.0f) + (b.m_y - a.m_y) * cos(PI / 3.0f));
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
			SDL_FillRect(window, &(window->clip_rect), SDL_MapRGB(window->format, 0, 0, 0));

//			vonKoch_rot(window, iterations, a, b);
			vonKoch_proj(window, iterations, a, b);
			if (flocon)
			{
				vonKoch_rot(window, iterations, b, c);
				vonKoch_rot(window, iterations, c, a);
			}

			// Met à jour le buffer de la fenêtre
			SDL_UpdateRect(window, 0, 0, 0, 0);
			// Flip le buffer pour l'envoyer vers l'écran (la fenêtre est configurée en double buffer)
			SDL_Flip(window);
		}

	}

	return EXIT_SUCCESS;
}