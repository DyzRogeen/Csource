#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include "graph.h"

#define WIN_WIDTH 640
#define WIN_HEIGHT 480

void renderSurface(SDL_Renderer* renderer, SDL_Surface* surface) {
	SDL_RenderClear(renderer);
	SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
	SDL_RenderCopy(renderer, texture, NULL, NULL);
	SDL_RenderPresent(renderer);
	SDL_DestroyTexture(texture);
}

int main(int argc, char **argv)
{
	SDL_Surface *window, *surface;
	SDL_Renderer* renderer;

	int quit = 0;
	SDL_Event e;
	point mousePos;

	// Initialisation des fenêtres
	{
		if (SDL_Init(SDL_INIT_VIDEO) < 0) {
			fprintf(stderr, "ERREUR - SDL_Init\n>>> %s\n", SDL_GetError());
			exit(EXIT_FAILURE);
		}

		atexit(SDL_Quit);
		window = SDL_CreateWindow("SDL2 Window", 750, SDL_WINDOWPOS_UNDEFINED, WIN_WIDTH, WIN_HEIGHT, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);

		if (window == NULL) {
			fprintf(stderr, "ERREUR - impossible de passer en : %dx%dx%d\n>>> %s\n", WIN_WIDTH, WIN_HEIGHT, 32, SDL_GetError());
			exit(EXIT_FAILURE);
		}

		surface = SDL_CreateRGBSurface(0, WIN_WIDTH, WIN_HEIGHT, 32, 0x00FF0000, 0x0000FF00, 0x000000FF, 0xFF000000);
		renderer = SDL_CreateRenderer(window, 0, 0);
	}
	
	SDL_Keycode key;
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
				key = e.key.keysym.sym;

				if (key == SDLK_ESCAPE)
					quit = 1;
				break;
			case SDL_MOUSEBUTTONDOWN:
				SDL_GetMouseState(&mousePos.x, &mousePos.y);
				break;
			}
		}

		if (0)
		{
			renderSurface(renderer, surface);
		}

	}

	SDL_FreeSurface(surface);
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);

	return EXIT_SUCCESS;
}