#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include "utils_SDL.h"

#include "../SDL2_image-2.6.3/include/SDL_image.h"

#define PI	3.14159265359

typedef struct sPoint{
	int x;
	int y;
}point;

Uint32* cpyIMG(SDL_Surface* img) {

	int w = img->w;
	int h = img->h;

	Uint32* cpy = (Uint32*)calloc(w * h, sizeof(Uint32));
	Uint32* pxl = (Uint32*)(img->pixels);

	for (int x = 0; x < w; x++)
		for (int y = 0; y < h; y++) *(cpy + x + y * w) = *(pxl + x + y * w);

	return cpy;

}

Uint32* getPxW(SDL_Surface* window, int x, int y) {

	return (Uint32*)(window->pixels) + x + y * window->w;

}

Uint32 getPixel(SDL_Surface* surface, int x, int y, int bpp) {

	Uint8* p = (Uint8*)(surface->pixels) + y * surface->pitch + x * bpp;

	switch (bpp) {
	case 1:
		return *p;

	case 2:
		return *(Uint16*)p;

	case 3:
		if (SDL_BYTEORDER == SDL_BIG_ENDIAN)
			return p[0] << 16 | p[1] << 8 | p[2];
		else
			return p[0] | p[1] << 8 | p[2] << 16;

	case 4:
		return p[0] << 16 | p[1] << 8 | p[2] | 100 << 24;

	default:
		return 0; /* shouldn't happen, but avoids warnings */
	}
}

void setMatrix(SDL_Surface* window, SDL_Surface* img) {

	int bpp = img->format->BytesPerPixel;
	int w = window->w;
	int h = window->h;

	printf("Processing...\n");
	for (int x = 0; x < w; x++)
		for (int y = 0; y < h; y++) *((Uint32*)(window->pixels) + x + y * window->w) = getPixel(img, x, y, 4);

	SDL_UpdateRect(window, 0, 0, 0, 0);
	SDL_Flip(window);

	printf("Done !\n");
	
}

void pixelize(SDL_Surface* window, int deg) {

	int deg2 = deg * deg;
	int w = window->w - deg;
	int h = window->h - deg;

	int PxSum[3];
	Uint8 PxSum8[4];
	Uint8* p;

	for (int x = 0; x < w; x += deg) {
		for (int y = 0; y < h; y += deg) {

			PxSum[0] = 0;
			PxSum[1] = 0;
			PxSum[2] = 0;

			for (int i = 0; i < deg; i++)
				for (int j = 0; j < deg; j++) {
					p = (Uint8*)getPxW(window, x + i, y + j);
					PxSum[0] += p[0];
					PxSum[1] += p[1];
					PxSum[2] += p[2];
				}

			PxSum8[0] = PxSum[0] / deg2;
			PxSum8[1] = PxSum[1] / deg2;
			PxSum8[2] = PxSum[2] / deg2;

			for (int i = 0; i < deg; i++)
				for (int j = 0; j < deg; j++) *getPxW(window, x + i, y + j) = *(Uint32*)PxSum8;

		}
	}

	SDL_UpdateRect(window, 0, 0, 0, 0);
	SDL_Flip(window);

}

void smooth(SDL_Surface* window, float pct) {

	float pct2 = pct * pct;
	Uint32* cpy = cpyIMG(window);
	Uint32* pxl = (Uint32*)(window->pixels);

	Uint8* p, *pij;

	int w = window->w;
	int h = window->h;

	for (int x = 0; x < w; x++) {
		for (int y = 0; y < h; y++) {

			for (int i = x - 1; i <= x + 1; i++) {

				if (i < 0 || i > w) continue;

				for (int j = y - 1; j <= y + 1; j++) {

					if (j < 0 || j > h || (i == x && j == y)) continue;

					p = (Uint8*)(pxl + x + y * w);
					pij = (Uint8*)(pxl + i + j * w);

					// p = (p + pij * pct) / (1 + pct)
					if (i == x || j == y) {
						p[0] = (Uint8)((p[0] + pij[0] * pct) / (1 + pct));
						p[1] = (Uint8)((p[1] + pij[1] * pct) / (1 + pct));
						p[2] = (Uint8)((p[2] + pij[2] * pct) / (1 + pct));
					}
					else {
						p[0] = (Uint8)((p[0] + pij[0] * pct2) / (1 + pct2));
						p[1] = (Uint8)((p[1] + pij[1] * pct2) / (1 + pct2));
						p[2] = (Uint8)((p[2] + pij[2] * pct2) / (1 + pct2));
					}

				}
			}

		}
	}

	free(cpy);

	SDL_UpdateRect(window, 0, 0, 0, 0);
	SDL_Flip(window);

}

void homogene(SDL_Surface* window, float pct) {

	float pct2 = pct * pct;
	Uint32* cpy = cpyIMG(window);
	Uint32* pxl = (Uint32*)(window->pixels);

	Uint8* p = NULL, * pij;
	Uint32* tab[8];

	int w = window->w;
	int h = window->h;

	int nP;
	int PxSum[3];
	Uint8 PxSum8[4];

	for (int x = 0; x < w; x++) {
		for (int y = 0; y < h; y++) {

			nP = 0;

			for (int i = x - 1; i <= x + 1; i++) {

				if (i < 0 || i > w) continue;

				for (int j = y - 1; j <= y + 1; j++) {

					if (j < 0 || j > h || (i == x && j == y)) continue;

					p = (Uint8*)(pxl + x + y * w);
					pij = (Uint8*)(pxl + i + j * w);

					// p = (p + pij * pct) / (1 + pct)
					if (p[0] < (1 + pct) * pij[0] && p[0] > (1 - pct) * pij[0] && p[1] < (1 + pct) * pij[1] && p[1] > (1 - pct) * pij[1] && p[2] < (1 + pct) * pij[2] && p[2] > (1 - pct) * pij[2]) {
						tab[nP] = pxl + i + j * w;
						nP++;
					}
				}
			}

			if (nP) {

				PxSum[0] = p[0];
				PxSum[1] = p[1];
				PxSum[2] = p[2];

				for (int ind = 0; ind < nP; ind++) {
					pij = (Uint8*)(tab[ind]);
					PxSum[0] += pij[0];
					PxSum[1] += pij[1];
					PxSum[2] += pij[2];
				}

				PxSum8[0] = PxSum[0] / (nP + 1);
				PxSum8[1] = PxSum[1] / (nP + 1);
				PxSum8[2] = PxSum[2] / (nP + 1);

				p = *(Uint32*)PxSum8;
				for (int ind = 0; ind < nP; ind++) *(tab[ind]) = p;
			}

		}
	}

	free(cpy);

	SDL_UpdateRect(window, 0, 0, 0, 0);
	SDL_Flip(window);

}

void negative(SDL_Surface* window) {

	int w = window->w;
	int h = window->h;
	Uint32* pxl = (Uint32*)(window->pixels);
	Uint8* p;

	for (int x = 0; x < w; x++)
		for (int y = 0; y < h; y++) {

			p = (Uint8*)(pxl + x + y * w);
			p[0] = 255 - p[0];
			p[1] = 255 - p[1];
			p[2] = 255 - p[2];

		}

	SDL_UpdateRect(window, 0, 0, 0, 0);
	SDL_Flip(window);

}

int main(int argc, char **argv)
{
	SDL_Surface * window;
	SDL_Surface * img;
	int quit = 0;
	SDL_Event e;
	point mousePos;

	if (SDL_Init(SDL_INIT_VIDEO) < 0) {
		fprintf(stderr, "ERREUR - SDL_Init\n>>> %s\n", SDL_GetError());
		exit(EXIT_FAILURE);
	}

	atexit(SDL_Quit);

	if (!(img = IMG_Load("camol.jpg"))) {
		printf("Unable to open jpg file.\n");
		exit(EXIT_FAILURE);
	}

	printf("IMG FORMAT : %dx%d\n", img->w, img->h);

	window = SDL_SetVideoMode(img->w, img->h, 32, SDL_HWSURFACE | SDL_DOUBLEBUF);

	if (window == NULL) {
		fprintf(stderr, "ERREUR - impossible de passer en : %dx%dx%d\n>>> %s\n", 640, 480, 32, SDL_GetError());
		exit(EXIT_FAILURE);
	}

	window->format->BytesPerPixel = 4;
	SDL_WM_SetCaption("JPEG", NULL);

	while (!quit)
	{
		while (SDL_PollEvent(&e))
		{
			switch (e.type)
			{
			case SDL_QUIT:
				SDL_FreeSurface(window);
				SDL_FreeSurface(img);
				quit = 1;
				break;
			case SDL_KEYDOWN:
				if (e.key.keysym.sym == SDLK_ESCAPE)
					quit = 1;
				if (e.key.keysym.sym == SDLK_SPACE)
					setMatrix(window, img);
				if (e.key.keysym.sym == SDLK_p)
					pixelize(window, 5);
				if (e.key.keysym.sym == SDLK_s)
					smooth(window, 1);
				if (e.key.keysym.sym == SDLK_h)
					homogene(window, 0.1);
				if (e.key.keysym.sym == SDLK_n)
					negative(window);
				if (e.key.keysym.sym == SDLK_r && IMG_SaveJPG(window, "cammile.jpg", 100))
					exit(EXIT_FAILURE);
				break;
			case SDL_MOUSEBUTTONDOWN:
				SDL_GetMouseState(&mousePos.x, &mousePos.y);
				break;
			}
		}

		if (0)
		{
			SDL_FillRect(window, &(window->clip_rect), SDL_MapRGB(window->format, 0, 0, 0));
			SDL_UpdateRect(window, 0, 0, 0, 0);
			SDL_Flip(window);
		}

	}

	return EXIT_SUCCESS;
}