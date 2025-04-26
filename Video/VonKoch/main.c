#define _CRT_SECURE_NO_WARNINGS 0
#include <stdio.h>
#include <stdlib.h>
#include "utils_SDL.h"
#include "video.h"

#include "../SDL2_image-2.6.3/include/SDL_image.h"

typedef struct sPoint{
	int x;
	int y;
}point;

int main(int argc, char **argv)
{

	SDL_Surface * window, *img;
	int quit = 0;
	SDL_Event e;
	SDLKey key;

	float filter[3] = {
		-0.5, 0, 0.5,
	};

	Uint32* pxlsH = NULL, * pxlsV = NULL;

	int gaussFilterDim = 11;
	float* filterGH = getGaussianDiffFilter(gaussFilterDim, 1);
	float* filterGV = getGaussianDiffFilter(gaussFilterDim, 0);
	printFilter(filterGH, gaussFilterDim, gaussFilterDim);
	printFilter(filterGV, gaussFilterDim, gaussFilterDim);

	if (SDL_Init(SDL_INIT_VIDEO) < 0) {
		fprintf(stderr, "ERREUR - SDL_Init\n>>> %s\n", SDL_GetError());
		exit(EXIT_FAILURE);
	}

	atexit(SDL_Quit);

	if (!(img = IMG_Load("exmpl.png"))) {
		printf("Unable to open jpg file.\n");
		exit(EXIT_FAILURE);
	}

	printf("IMG FORMAT : %dx%d\n", img->w, img->h);

	window = SDL_SetVideoMode(img->w, img->h, 32, SDL_HWSURFACE | SDL_DOUBLEBUF);
	window->clip_rect.x = 300;

	if (window == NULL) {
		fprintf(stderr, "ERREUR - impossible de passer en : %dx%dx%d\n>>> %s\n", img->w, img->h, 32, SDL_GetError());
		exit(EXIT_FAILURE);
	}

	SDL_WM_SetCaption("Video", NULL);

	mapPxls(window, img);

	SDL_Flip(window);

	while (!quit)
	{
		while (SDL_PollEvent(&e))
		{
			switch (e.type)
			{
			case SDL_QUIT:
				SDL_FreeSurface(window);
				free(img);
				quit = 1;
				break;
			case SDL_KEYDOWN:

				mapPxls(window, img);


				key = e.key.keysym.sym;
				if (key == SDLK_ESCAPE)
					quit = 1;
				if (key == SDLK_l)
					mapPxls(window, img);
				if (key == SDLK_b)
					blackWhite(window);
				if (key == SDLK_g)
					gaussianNoiseReducer(window);
				if (key == SDLK_1) {
					convolution(window, filter, 3, 1);
					
					free(pxlsV);
					pxlsV = copyPxls(window);
				}
				if (key == SDLK_2) {
					convolution(window, filter, 1, 3);

					free(pxlsH);
					pxlsH = copyPxls(window);
				}
				if (key == SDLK_3) normIMGs(window, pxlsH, pxlsV);
				if (key == SDLK_4) {
					convolution(window, filterGH, gaussFilterDim, gaussFilterDim);

					free(pxlsV);
					pxlsV = copyPxls(window);
				}
				if (key == SDLK_5) {
					convolution(window, filterGV, gaussFilterDim, gaussFilterDim);

					free(pxlsH);
					pxlsH = copyPxls(window);
				}
				if (key == SDLK_UP) gaussFilterDim+=2;
				if (key == SDLK_DOWN) gaussFilterDim-=2;
				if (key == SDLK_UP || key == SDLK_DOWN) {
					free(filterGH);
					free(filterGV);

					filterGH = getGaussianDiffFilter(gaussFilterDim, 1);
					filterGV = getGaussianDiffFilter(gaussFilterDim, 0);
					printFilter(filterGH, gaussFilterDim, gaussFilterDim);
					printFilter(filterGV, gaussFilterDim, gaussFilterDim);
				}

				SDL_Flip(window);

				break;
			}
		}

	}


	free(pxlsH);
	free(pxlsV);
	free(filterGH);
	free(filterGV);

	return EXIT_SUCCESS;
}