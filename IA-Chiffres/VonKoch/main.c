#define _CRT_SECURE_NO_WARNINGS 0

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include "utils_SDL.h"
#include "graph.h"
#include "NNetwork.h"

#define PI	3.14159265359
#define WIDTH 640
#define HEIGHT 640
#define RESOLUTION 8

int main(int argc, char **argv)
{

	SDL_Surface * window;
	int quit = 0;
	SDL_Event e;

	point mousePos;
	int mouseState = 0;
	int number;
	int nLine = 1;

	graph* g = initGraph(RESOLUTION, WIDTH, HEIGHT);

	int init[4] = { RESOLUTION * RESOLUTION, 32, 10 };

	weight** w = initWeight(init, 3);

	learn(w);
	stats(w);
	
	//network* nN = initNetwork((int*) { RESOLUTION* RESOLUTION, 32, 10 }, 3);

	if (SDL_Init(SDL_INIT_VIDEO) < 0) {
		fprintf(stderr, "ERREUR - SDL_Init\n>>> %s\n", SDL_GetError());
		exit(EXIT_FAILURE);
	}

	atexit(SDL_Quit);
	window = SDL_SetVideoMode(WIDTH, HEIGHT, 32, SDL_HWSURFACE | SDL_DOUBLEBUF);

	if (window == NULL) {
		fprintf(stderr, "ERREUR - impossible de passer en : %dx%dx%d\n>>> %s\n", WIDTH, HEIGHT, 32, SDL_GetError());
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
				else if (e.key.keysym.sym == SDLK_r)
					resetGraph(window, g);
				else if (e.key.keysym.sym == SDLK_g)
					guess(w, g->vector, RESOLUTION * RESOLUTION, 1);
				else if (e.key.keysym.sym == SDLK_s)
					stats(w);
				else if (e.key.keysym.sym == SDLK_UP || e.key.keysym.sym == SDLK_DOWN) {

					nLine += (nLine > 1 && e.key.keysym.sym == SDLK_DOWN) ? -1 : 1;

					nLine -= (readV(g, nLine) < 0) ? 1 : 0;
					affGraph(window, g);
					SDL_UpdateRect(window, 0, 0, 0, 0);
					SDL_Flip(window);

				}
				else if (g, e.key.keysym.sym >= SDLK_0 && g, e.key.keysym.sym <= SDLK_9) {
					registerV(g, e.key.keysym.sym - SDLK_0);
					resetGraph(window, g);
				}
				break;
			case SDL_MOUSEBUTTONDOWN:
				mouseState = 1;

				break;
			case SDL_MOUSEBUTTONUP:
				mouseState = 0;
				printGraph(g);
				break;
			}
		}

		if (mouseState)
		{
			SDL_GetMouseState(&mousePos.x, &mousePos.y);

			applicatePressure(g, mousePos.x, mousePos.y);

			affGraph(window, g);
			SDL_UpdateRect(window, 0, 0, 0, 0);
			SDL_Flip(window);
		}

	}

	freeGraph(g);
	freeWeight(w);

	return EXIT_SUCCESS;
}