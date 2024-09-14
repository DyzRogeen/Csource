#define PI	3.14159265359
#define _CRT_SECURE_NO_WARNINGS 0
#define FILE_PATH "../../LENIA/VonKoch/funcB.txt"

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include "utils_SDL.h"
#include "main.h"

typedef struct sPoint{
	int x;
	int y;
}point;

void writeFile(float* func, int len, int x_inf) {

	FILE* f = fopen(FILE_PATH, "w");

	if (!f) {
		printf("File could not be opened !\n");
		return NULL;
	}

	fprintf(f, "%d %d\n", len, x_inf);

	for (int i = 0; i < len; i++) {
		fprintf(f, "%f;", func[i]);
	}

	printf("Saved.\n");

	fclose(f);

}

void readFile(float* func, int x_inf_curr, int x_sup_curr) {

	FILE* f = fopen(FILE_PATH, "r");

	if (!f) {
		printf("File could not be opened !\n");
		return NULL;
	}

	int len, x_inf;

	fscanf(f, "%d %d\n", &len, &x_inf);

	for (int i = 0; i < len; i++) {
		if (i + x_inf >= x_inf_curr && i + x_inf < x_sup_curr)
			fscanf(f, "%f;", &func[i + x_inf - x_inf_curr]);
	}

	printf("Loaded.\n");

	fclose(f);

}

void drawFunc(SDL_Surface* window, float* func,int x_sup, int x_inf, int y_inf, int x_mid, int y_mid, float scale_x, float scale_y) {

	int width = window->w;
	int height = window->h;

	SDL_FillRect(window, &(window->clip_rect), SDL_MapRGB(window->format, 0, 0, 0));
	for (int i = 0; i < x_sup - x_inf - 1; i++) {
		_SDL_DrawLine(window, i * scale_x, height - (func[i] - y_inf) * scale_y, (i + 1) * scale_x, height - (func[i + 1] - y_inf) * scale_y, 255, 255, 255);
	}

	_SDL_DrawLine(window, 0, y_mid, width, y_mid, 255, 0, 0);
	_SDL_DrawLine(window, x_mid, 0, x_mid, window->h, 0, 0, 255);

	// Graduations (peut être automatisé)
	_SDL_DrawLine(window, (3 * x_mid - width) / 2, y_mid + 5, (3 * x_mid - width) / 2, y_mid - 5, 255, 0, 0);
	_SDL_DrawLine(window, (width + x_mid) / 2, y_mid + 5, (width + x_mid) / 2, y_mid - 5, 255, 0, 0);
	_SDL_DrawLine(window, (5 * x_mid - width) / 4, y_mid + 2, (5 * x_mid - width) / 4, y_mid - 2, 255, 0, 0);
	_SDL_DrawLine(window, (width + 3 * x_mid) / 4, y_mid + 2, (width + 3 * x_mid) / 4, y_mid - 2, 255, 0, 0);
	_SDL_DrawLine(window, (7 * x_mid - 3 * width) / 4, y_mid + 2, (7 * x_mid - 3 * width) / 4, y_mid - 2, 255, 0, 0);
	_SDL_DrawLine(window, (3 * width + x_mid) / 4, y_mid + 2, (3 * width + x_mid) / 4, y_mid - 2, 255, 0, 0);

	_SDL_DrawLine(window, x_mid - 5, (3 * y_mid - height) / 2, x_mid + 5, (3 * y_mid - height) / 2, 0, 0, 255);
	_SDL_DrawLine(window, x_mid - 5, (height + y_mid) / 2, x_mid + 5, (height + y_mid) / 2, 0, 0, 255);
	_SDL_DrawLine(window, x_mid - 2, (5 * y_mid - height) / 4, x_mid + 2, (5 * y_mid - height) / 4, 0, 0, 255);
	_SDL_DrawLine(window, x_mid - 2, (height + 3 * y_mid) / 4, x_mid + 2, (height + 3 * y_mid) / 4, 0, 0, 255);
	_SDL_DrawLine(window, x_mid - 2, (7 * y_mid - 3 * height) / 4, x_mid + 2, (7 * y_mid - 3 * height) / 4, 0, 0, 255);
	_SDL_DrawLine(window, x_mid - 2, (3 * height + y_mid) / 4, x_mid + 2, (3 * height + y_mid) / 4, 0, 0, 255);

	SDL_UpdateRect(window, 0, 0, 0, 0);
	SDL_Flip(window);
}

int main(int argc, char **argv)
{
	SDL_Surface * window;
	int quit = 0;
	SDL_Event e;
	point mPos;
	int mouseDown = 0;
	float scale_x, scale_y;
	float x, y;
	
	int x_inf = 0;
	int x_sup = 100;
	int y_inf = -1;
	int y_sup = 1;

	x_sup++;

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

	float* func = (float*)calloc(x_sup - x_inf, sizeof(float));

	for (int i = 0; i < x_sup - x_inf; i++) func[i] = 0;

	scale_x = (float)window->w / (x_sup - x_inf - 1);
	scale_y = (float)window->h / (y_sup - y_inf);

	int x_mid = (float)window->w * x_inf / (x_inf - x_sup);
	int y_mid = window->h - (float)window->h * y_inf / (y_inf - y_sup);
	drawFunc(window, func, x_sup, x_inf, y_inf, x_mid, y_mid, scale_x, scale_y);

	Uint32* pixels = (Uint32*)window->pixels;

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
				if (e.key.keysym.sym == SDLK_s)
					writeFile(func, x_sup - x_inf, x_inf);
				if (e.key.keysym.sym == SDLK_l) {
					readFile(func, x_inf, x_sup);
					drawFunc(window, func, x_sup, x_inf, y_inf, x_mid, y_mid, scale_x, scale_y);
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
			SDL_GetMouseState(&mPos.x, &mPos.y);

			x = (float) mPos.x / scale_x;
			y = (float) (window->h - mPos.y) / scale_y;

			func[(int)(x + 0.5)] = y + y_inf;

			drawFunc(window, func, x_sup, x_inf, y_inf, x_mid, y_mid, scale_x, scale_y);
			
		}

	}

	free(func);

	return EXIT_SUCCESS;
}