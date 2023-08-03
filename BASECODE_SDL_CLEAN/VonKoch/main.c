#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include "utils_SDL.h"

#define PI	3.14159265359

typedef struct sPoint{
	int m_x;
	int m_y;
}point;

int main(int argc, char **argv)
{
	SDL_Surface * window;
	int quit = 0;
	SDL_Event e;
	point mousePos;

	if (SDL_Init(SDL_INIT_VIDEO) < 0) {
		fprintf(stderr, "ERREUR - SDL_Init\n>>> %s\n", SDL_GetError());
		exit(EXIT_FAILURE);
	}

	atexit(SDL_Quit);
	window = SDL_SetVideoMode(640, 480, 32, SDL_HWSURFACE | SDL_DOUBLEBUF);

	if (window == NULL) {
		fprintf(stderr, "ERREUR - impossible de passer en : %dx%dx%d\n>>> %s\n", 640, 480, 32, SDL_GetError());
		exit(EXIT_FAILURE);
	}

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
				break;
			case SDL_MOUSEBUTTONDOWN:
				SDL_GetMouseState(&mousePos.m_x, &mousePos.m_y);
				break;
			}
		}

		if (1)
		{
			SDL_FillRect(window, &(window->clip_rect), SDL_MapRGB(window->format, 0, 0, 0));
			SDL_UpdateRect(window, 0, 0, 0, 0);
			SDL_Flip(window);
		}

	}

	return EXIT_SUCCESS;
}