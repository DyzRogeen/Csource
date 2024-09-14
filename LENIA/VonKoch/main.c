#define PI	3.14159265359
#define _CRT_SECURE_NO_WARNINGS 0

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include "utils_SDL.h"
#include "graph.h"

typedef struct sPoint {
	int x;
	int y;
}point;

float* readFile(char* fileName) {

	FILE* f = fopen(fileName, "r");

	if (!f) {
		printf("File could not be opened !\n");
		return NULL;
	}

	int len, tmp;

	fscanf(f, "%d %d\n", &len, &tmp);

	float* growthFunc = (float*)calloc(len, sizeof(len));

	for (int i = 0; i < len; i++) {
		fscanf(f, "%f;", &growthFunc[i]);
	}

	printf("Loaded.\n");

	fclose(f);

	return growthFunc;

}

int main(int argc, char** argv)
{

	SDL_Surface* window;
	int quit = 0;
	SDL_Event evt;
	point mousePos, mousePos0;
	int mouseDown = 0;

	mousePos.x = 0;
	mousePos.y = 0;
	mousePos0.x = 0;
	mousePos0.y = 0;


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

	SDL_WM_SetCaption("LENIA", NULL);

	int filterDim[3] = { 17, 23, 13 };
	float* filter[3] = {
		(float*)calloc(filterDim[0] * filterDim[0], sizeof(float)),
		(float*)calloc(filterDim[1] * filterDim[1], sizeof(float)),
		(float*)calloc(filterDim[2] * filterDim[2], sizeof(float))
	};
	donutFilter(filter[0], filterDim[0], 4, 3);
	donutFilter(filter[0], filterDim[0], 9, 1);
	donutFilter(filter[1], filterDim[1], 3, 2);
	donutFilter(filter[1], filterDim[1], 6, 3);
	donutFilter(filter[1], filterDim[1], 10, 2);
	donutFilter(filter[2], filterDim[2], 3, 1);
	donutFilter(filter[2], filterDim[2], 7, 3);

	float* growthFunc[3] = { readFile("funcR.txt"), readFile("funcG.txt"), readFile("funcB.txt") };
	
	Uint32* pixels = window->pixels;
	Uint8* pxl;
	for (int ind = 0; ind < 3; ind++) {
		int decal = 110 - (filterDim[ind] - 1) / 2;
		for (int i = 0; i < filterDim[ind]; i++) {
			printf("[");
			for (int j = 0; j < filterDim[ind]; j++) {
				pxl = (Uint8*)(pixels + (i + decal) + (j + decal) * 640);

				Uint8 f = filter[ind][i + j * filterDim[ind]] * 255;
				pxl[ind] = f;
				printf(" %.2f", filter[ind][i + j * filterDim[ind]]);

			}
			printf(" ]\n");
		}
		printf("\n");
	}

	SDL_UpdateRect(window, 0, 0, 0, 0);
	SDL_Flip(window);

	while (!quit)
	{
		while (SDL_PollEvent(&evt))
		{
			switch (evt.type)
			{
			case SDL_QUIT:
				quit = 1;
				break;
			case SDL_KEYDOWN:
				if (evt.key.keysym.sym == SDLK_ESCAPE)
					quit = 1;
				else if (evt.key.keysym.sym == SDLK_s) {
					step(window, growthFunc, filter, filterDim);
					SDL_UpdateRect(window, 0, 0, 0, 0);
					SDL_Flip(window);
				}
				else if (evt.key.keysym.sym == SDLK_f) {
					fillRandom(window);
					SDL_UpdateRect(window, 0, 0, 0, 0);
					SDL_Flip(window);
				}
				else if (evt.key.keysym.sym == SDLK_c) {
					SDL_FillRect(window, &(window->clip_rect), SDL_MapRGB(window->format, 0, 0, 0));
					SDL_UpdateRect(window, 0, 0, 0, 0);
					SDL_Flip(window);
				}

				break;
			case SDL_MOUSEBUTTONDOWN:
				mouseDown = 1;
				break;
			case SDL_MOUSEBUTTONUP:
				mouseDown = 0;
				break;
			}
		}

		if (mouseDown)
		{
			//SDL_FillRect(window, &(window->clip_rect), SDL_MapRGB(window->format, 0, 0, 0));

			SDL_GetMouseState(&mousePos.x, &mousePos.y);

			if (norm(mousePos0.x - mousePos.x, mousePos0.y - mousePos.y) < 3) continue;

			mousePos0.x = mousePos.x;
			mousePos0.y = mousePos.y;

			gaussianDot(window, mousePos.x, mousePos.y, 5, SDL_MapRGB(window->format, 255, 255, 255));
			SDL_UpdateRect(window, 0, 0, 0, 0);
			SDL_Flip(window);
		}

	}

	free(filter[0]);
	free(filter[1]);
	free(filter[2]);
	free(growthFunc[0]);
	free(growthFunc[1]);
	free(growthFunc[2]);

	return EXIT_SUCCESS;
}