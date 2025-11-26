#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include "mandelbrot.h"
#include "graph.h"

#define WIN_WIDTH 640
#define WIN_HEIGHT 640

void renderSurface(SDL_Renderer* renderer, SDL_Surface* surface) {
	SDL_RenderClear(renderer);
	SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
	SDL_RenderCopy(renderer, texture, NULL, NULL);
	SDL_RenderPresent(renderer);
	SDL_DestroyTexture(texture);
}

void refreshJulia(SDL_Renderer* renderer, SDL_Surface* surface, pointf offset, double UPP, pointf c, float power, float nb_iter) {
	
	drawJulia(surface, offset, UPP, c, power, nb_iter);
	drawAxis(surface, offset, UPP);
	drawCursor(surface);
	
	renderSurface(renderer, surface);
	
}

void refreshMandelbrot(SDL_Renderer* renderer, SDL_Surface* surface, pointf offset, double UPP, pointf c, float power, float nb_iter) {

	drawMandelbrot(surface, offset, UPP, power, nb_iter);
	drawAxis(surface, offset, UPP);
	drawC(surface, offset, UPP, c);
	drawCursor(surface);

	renderSurface(renderer, surface);

}

int main(int argc, char **argv)
{

	SDL_Surface* windowJulia,* windowMandelbrot;
	SDL_Surface* surfaceJulia, *surfaceMandelbrot;
	SDL_Renderer* rendererJulia, *rendererMandelbrot;

	int quit = 0;
	SDL_Event e;
	point mousePos = { 0, 0 }, mousePrev = { 0, 0 };
	int mouseDown = 0, ctrl_down = 0, update_Julia = 1, update_Mandelbrot = 1;

	float power = 2;

	// Units Per Pixel (scale)
	double UPP_Julia, UPP_Mandelbrot;
	UPP_Julia = UPP_Mandelbrot = 3.f / WIN_WIDTH;

	pointf offset_Julia = { 0, 0 }, offset_Mandelbrot = { -0.75, 0 }, offsetPrev = { 0, 0 };

	int nb_iter = 256;
	pointf c = { -0.75, 0 };

	// Initialisation des fenêtres
	{
		if (SDL_Init(SDL_INIT_VIDEO) < 0) {
			fprintf(stderr, "ERREUR - SDL_Init\n>>> %s\n", SDL_GetError());
			exit(EXIT_FAILURE);
		}

		atexit(SDL_Quit);
		windowJulia = SDL_CreateWindow("Julia", 750, SDL_WINDOWPOS_UNDEFINED, WIN_WIDTH, WIN_HEIGHT, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
		windowMandelbrot = SDL_CreateWindow("Mandelbrot", 100, SDL_WINDOWPOS_UNDEFINED, WIN_WIDTH, WIN_HEIGHT, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);

		if (windowJulia == NULL || windowMandelbrot == NULL) {
			fprintf(stderr, "ERREUR - impossible de passer en : %dx%dx%d\n>>> %s\n", WIN_WIDTH, WIN_HEIGHT, 32, SDL_GetError());
			exit(EXIT_FAILURE);
		}
		surfaceJulia = SDL_CreateRGBSurface(0, WIN_WIDTH, WIN_HEIGHT, 32, 0x00FF0000, 0x0000FF00, 0x000000FF, 0xFF000000);
		rendererJulia = SDL_CreateRenderer(windowJulia, 0, 0);
		surfaceMandelbrot = SDL_CreateRGBSurface(0, WIN_WIDTH, WIN_HEIGHT, 32, 0x00FF0000, 0x0000FF00, 0x000000FF, 0xFF000000);
		rendererMandelbrot = SDL_CreateRenderer(windowMandelbrot, 0, 0);
	}

	SDL_Keycode key;
	Uint32 winId;
	while (!quit)
	{
		while (SDL_PollEvent(&e))
		{
			winId = e.window.windowID;

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

				case SDLK_UP:
					if (winId == 1) UPP_Julia *= 0.75;
					else if (winId == 2) UPP_Mandelbrot *= 0.75;
					printf("UPP_J : %Lf | UPP_M : %Lf\n", UPP_Julia, UPP_Mandelbrot);
					break;
				
				case SDLK_DOWN:
					if (winId == 1) {
						UPP_Julia /= 0.75;
						if (UPP_Julia >= 1) UPP_Julia = 1;
					}
					else if (winId == 2) {
						UPP_Mandelbrot /= 0.75;
						if (UPP_Mandelbrot >= 1) UPP_Mandelbrot = 1;
					}
					printf("UPP_J : %Lf | UPP_M : %Lf\n", UPP_Julia, UPP_Mandelbrot);
					break;

				case SDLK_LEFT:
					nb_iter /= 1.2;
					if (nb_iter < 1) nb_iter = 1;
					printf("%d ITERATIONS\n", nb_iter);
					update_Julia = update_Mandelbrot = 1;
					break;

				case SDLK_RIGHT:
					nb_iter *= 1.2;
					printf("%d ITERATIONS\n", nb_iter);
					update_Julia = update_Mandelbrot = 1;
					break;
				
				case SDLK_LCTRL:
					ctrl_down = 1;
					if (winId == 2) mouseDown = 2;
					break;

				}
				
				if (winId == 1) update_Julia = 1;
				else if (winId == 2) update_Mandelbrot = 1;
				break;

			case SDL_KEYUP:
				key = e.key.keysym.sym;
				if (key == SDLK_LCTRL) {
					ctrl_down = 0;
					mouseDown = 0;
				}
				break;

			case SDL_MOUSEBUTTONDOWN:
				mouseDown = winId;

				if (winId == 1) offsetPrev = offset_Julia;
				else if (winId == 2 && !ctrl_down) offsetPrev = offset_Mandelbrot;
				if (!ctrl_down) SDL_GetMouseState(&mousePrev.x, &mousePrev.y);
				mousePrev.x *= -1;
				break;

			case SDL_MOUSEBUTTONUP:
				mouseDown = 0;
				break;

			}
		}

		if (mouseDown) {
			SDL_GetMouseState(&mousePos.x, &mousePos.y);
			mousePos.x *= -1;
			if (mouseDown == 1) offset_Julia = sumf(offsetPrev, scale(sum(mousePrev, mousePos, -1), UPP_Julia), 1);
			else if (mouseDown == 2) {
				if (ctrl_down) {
					c.x = UPP_Mandelbrot * (-mousePos.x - WIN_WIDTH / 2.f) + offset_Mandelbrot.x;
					c.y = UPP_Mandelbrot * (mousePos.y - WIN_HEIGHT / 2.f) - offset_Mandelbrot.y;
					update_Julia = 1;
				}
				else offset_Mandelbrot = sumf(offsetPrev, scale(sum(mousePrev, mousePos, -1), UPP_Mandelbrot), 1);
			}
		}

		if (mouseDown == 1 || update_Julia)
		{
			refreshJulia(rendererJulia, surfaceJulia, offset_Julia, UPP_Julia, c, power, nb_iter);

			update_Julia = 0;
		}

		if (mouseDown == 2 || update_Mandelbrot)
		{
			refreshMandelbrot(rendererMandelbrot, surfaceMandelbrot, offset_Mandelbrot, UPP_Mandelbrot, c, power, nb_iter);

			update_Mandelbrot = 0;
		}

	}
	SDL_FreeSurface(surfaceJulia);
	SDL_DestroyRenderer(rendererJulia);
	SDL_DestroyWindow(windowJulia);

	SDL_FreeSurface(surfaceMandelbrot);
	SDL_DestroyRenderer(rendererMandelbrot);
	SDL_DestroyWindow(windowMandelbrot);

	return EXIT_SUCCESS;
}