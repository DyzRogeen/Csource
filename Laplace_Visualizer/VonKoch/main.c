#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include "../../Common_Libs/SDL-1.2.15/include/SDL.h"
#include "graph.h"

#define PI	3.14159265359
#define WIN_WIDTH 640
#define WIN_HEIGHT 640

#define T_MAX 100

int main(int argc, char **argv)
{
	SDL_Surface * window;
	int quit = 0;
	SDL_Event e;

	point mousePos, mousePosTmp;
	mousePos.x = 0;
	mousePos.y = 0;

	// Pour la navigation
	point last_offset, drag_ref;
	drag_ref = mousePos;

	int mouse_down = 0;

	// Pour la mise à jour de l'affichage
	int changed = 1;
	int mouse_moved = 1;

	if (SDL_Init(SDL_INIT_VIDEO) < 0) {
		fprintf(stderr, "ERREUR - SDL_Init\n>>> %s\n", SDL_GetError());
		exit(EXIT_FAILURE);
	}

	atexit(SDL_Quit);
	window = SDL_SetVideoMode(WIN_WIDTH, WIN_HEIGHT, 32, SDL_HWSURFACE | SDL_DOUBLEBUF);

	if (window == NULL) {
		fprintf(stderr, "ERREUR - impossible de passer en : %dx%dx%d\n>>> %s\n", WIN_WIDTH, WIN_HEIGHT, 32, SDL_GetError());
		exit(EXIT_FAILURE);
	}

	SDL_WM_SetCaption("VonKoch", NULL);

	// Création de l'objet graph
	Graph* graph = malloc(sizeof(Graph));
	graph->pixels = window->pixels;
	graph->width = WIN_WIDTH;
	graph->height = WIN_HEIGHT;

	// Centrer l'origine
	graph->offset.x = WIN_WIDTH / 2;
	graph->offset.y = WIN_HEIGHT / 2;
	last_offset = graph->offset;

	// Zoom
	graph->zoom = 1.f;

	// Echelles ([-2 ; 2] sur les deux axes)
	graph->scaleX = 4; 
	graph->scaleY = 4;

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
				if (e.key.keysym.sym == SDLK_ESCAPE) quit = 1;
				if (e.key.keysym.sym == SDLK_UP) {
					graph->zoom += 0.5;
					changed = 1;
				}
				if (e.key.keysym.sym == SDLK_DOWN) {
					graph->zoom -= graph->zoom <= 0.5 ? 0 : 0.5;
					changed = 1;
				}
				break;
			case SDL_MOUSEBUTTONDOWN:
				if (!mouse_down) SDL_GetMouseState(&drag_ref.x, &drag_ref.y);
				mouse_down = 1;
				break;
			case SDL_MOUSEBUTTONUP:
				mouse_down = 0;
				last_offset = graph->offset;
				break;
			}
		}

		if (1)
		{

			if (mouse_down) {
				SDL_GetMouseState(&mousePos.x, &mousePos.y);
				graph->offset.x = last_offset.x + mousePos.x - drag_ref.x;
				graph->offset.y = last_offset.y + mousePos.y - drag_ref.y;
			} else {
				mousePosTmp = mousePos;
				SDL_GetMouseState(&mousePos.x, &mousePos.y);
				mouse_moved = mousePosTmp.x != mousePos.x || mousePosTmp.y != mousePos.y;
			}

			if (mouse_down || changed || mouse_moved) {

				SDL_FillRect(window, &(window->clip_rect), SDL_MapRGB(window->format, 0, 0, 0));
				drawGrid(graph);
				if (mouse_down || changed || mouse_moved) {
					drawCurve(graph, mousePos, T_MAX);
					mouse_moved = 0;
				}
				SDL_Flip(window);

				changed = 0;
			}

		}

	}

	return EXIT_SUCCESS;
}