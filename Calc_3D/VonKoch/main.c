#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include "graph.h"

#define WIN_WIDTH 640
#define WIN_HEIGHT 480

void renderSurface(SDL_Renderer* renderer, SDL_Surface* surface) {
	Uint32* pxls = surface->pixels;
	SDL_RenderClear(renderer);
	SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
	SDL_RenderCopy(renderer, texture, NULL, NULL);
	SDL_RenderPresent(renderer);
	SDL_DestroyTexture(texture);
	for (int i = 0; i < WIN_WIDTH * WIN_HEIGHT; i++) pxls[i] = 0;
}

void drawshit(SDL_Surface* surface, camera* cam) {

	drawGrid(surface, *cam);

	//point p1 = pojectPoint(*cam, (point3) { 0, 0, 0 });
	////printf("(%d, %d)\n", p1.x, p1.y);
	//point p2 = pojectPoint(*cam, (point3) { 1, 0, 0 });
	////printf("(%d, %d)\n", p2.x, p2.y);
	//point p3 = pojectPoint(*cam, (point3) { 0, 1, 0 });
	//point p4 = pojectPoint(*cam, (point3) { 0, 0, 1 });

	//drawline(surface, p1, p2, C_RED);
	//drawline(surface, p1, p3, C_BLUE);
	//drawline(surface, p1, p4, C_GREEN);
	float r = 10;
	int w2 = WIN_WIDTH / 2, h2 = WIN_HEIGHT / 2;
	point P1 = pojectPoint(*cam, (point3) { r, r, r }, w2, h2);
	point P2 = pojectPoint(*cam, (point3) { -r, r, r }, w2, h2);
	point P3 = pojectPoint(*cam, (point3) { r, -r, r }, w2, h2);
	point P4 = pojectPoint(*cam, (point3) { -r, -r, r }, w2, h2);
	point P5 = pojectPoint(*cam, (point3) { r, r, -r }, w2, h2);
	point P6 = pojectPoint(*cam, (point3) { -r, r, -r }, w2, h2);
	point P7 = pojectPoint(*cam, (point3) { r, -r, -r }, w2, h2);
	point P8 = pojectPoint(*cam, (point3) { -r, -r, -r }, w2, h2);

	drawline(surface, P1, P2, C_WHITE);
	drawline(surface, P1, P3, C_WHITE);
	drawline(surface, P1, P5, C_WHITE);
	drawline(surface, P8, P4, C_WHITE);
	drawline(surface, P8, P6, C_WHITE);
	drawline(surface, P8, P7, C_WHITE);
	drawline(surface, P2, P4, C_WHITE);
	drawline(surface, P2, P6, C_WHITE);
	drawline(surface, P7, P3, C_WHITE);
	drawline(surface, P7, P5, C_WHITE);
	drawline(surface, P3, P4, C_WHITE);
	drawline(surface, P5, P6, C_WHITE);

}

int main(int argc, char **argv)
{
	SDL_Surface *window, *surface;
	SDL_Renderer* renderer;

	int quit = 0;
	SDL_Event e;

	point mousePos, mousePrev = { 0, 0 };
	int mouseDown = 0, ctrlMode = 0;

	point3 posTmp = { 0 ,0 ,0 };
	float lonTmp = 0, latTmp = 0;

	camera* cam = initCamera((point3) { -50, 0, 0 }, 0, 0, 1.f / 100);

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

	updateProjectionPlane(cam);

	drawshit(surface, cam);
	renderSurface(renderer, surface);
	
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

				switch (key) {

				case SDLK_ESCAPE:
					quit = 1;
					break;

				case SDLK_LCTRL:
					if (!ctrlMode) {
						SDL_GetMouseState(&mousePrev.x, &mousePrev.y);
						posTmp = cam->position;
						lonTmp = cam->lon;
						latTmp = cam->lat;
					}
					ctrlMode = 1;
					break;
				}

				break;

			case SDL_KEYUP:
				key = e.key.keysym.sym;

				switch (key) {
				case SDLK_LCTRL:
					SDL_GetMouseState(&mousePrev.x, &mousePrev.y);
					ctrlMode = 0;
					break;
				}

				break;

			case SDL_MOUSEBUTTONDOWN:
				mouseDown = 1;
				SDL_GetMouseState(&mousePrev.x, &mousePrev.y);
				if (ctrlMode) {
					lonTmp = cam->lon;
					latTmp = cam->lat;
					break;
				}
				posTmp = cam->position;
				break;

			case SDL_MOUSEBUTTONUP:
				mouseDown = 0;
				break;
			}
		}

		if (mouseDown)
		{
			SDL_GetMouseState(&mousePos.x, &mousePos.y);

			// Rotation
			if (ctrlMode) {
				cam->lon = lonTmp - (float)(mousePrev.x - mousePos.x) * PI / WIN_WIDTH;
				cam->lat = latTmp + (float)(mousePrev.y - mousePos.y) * PI / WIN_HEIGHT;
			}
			// Translation
			else {
				point3 mov = sum(scale(cam->u, (float)(mousePrev.x - mousePos.x) / 20), scale(cam->v, (float)(mousePos.y - mousePrev.y) / 20));
				cam->position = sum(posTmp, mov);
			}

			updateProjectionPlane(cam);
			drawshit(surface, cam);
			SDL_RenderClear(renderer);


			renderSurface(renderer, surface);
		}

	}

	SDL_FreeSurface(surface);
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);

	return EXIT_SUCCESS;
}