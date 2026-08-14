#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "graph.h"
#include "../../Common_Libs/SDL2_image-2.6.3/include/SDL_image.h"

#define WIN_WIDTH 860
#define WIN_HEIGHT 720

void renderSurface(SDL_Renderer* renderer, SDL_Surface* surface) {
	Uint32* pxls = surface->pixels;
	SDL_RenderClear(renderer);
	SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
	SDL_RenderCopy(renderer, texture, NULL, NULL);
	SDL_RenderPresent(renderer);
	SDL_DestroyTexture(texture);
	for (int i = 0; i < WIN_WIDTH * WIN_HEIGHT; i++) pxls[i] = 0;
}

void drawshit(SDL_Surface* surface, cam* c) {


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
	int w2 = WIN_WIDTH, h2 = WIN_HEIGHT;
	point2 P1 = projectPoint(c, (point3) { r, r, r }, w2, h2);
	point2 P2 = projectPoint(c, (point3) { -r, r, r }, w2, h2);
	point2 P3 = projectPoint(c, (point3) { r, -r, r }, w2, h2);
	point2 P4 = projectPoint(c, (point3) { -r, -r, r }, w2, h2);
	point2 P5 = projectPoint(c, (point3) { r, r, -r }, w2, h2);
	point2 P6 = projectPoint(c, (point3) { -r, r, -r }, w2, h2);
	point2 P7 = projectPoint(c, (point3) { r, -r, -r }, w2, h2);
	point2 P8 = projectPoint(c, (point3) { -r, -r, -r }, w2, h2);

	drawLine(surface, P1, P2, WHITE);
	drawLine(surface, P1, P3, WHITE);
	drawLine(surface, P1, P5, WHITE);
	drawLine(surface, P8, P4, WHITE);
	drawLine(surface, P8, P6, WHITE);
	drawLine(surface, P8, P7, WHITE);
	drawLine(surface, P2, P4, WHITE);
	drawLine(surface, P2, P6, WHITE);
	drawLine(surface, P7, P3, WHITE);
	drawLine(surface, P7, P5, WHITE);
	drawLine(surface, P3, P4, WHITE);
	drawLine(surface, P5, P6, WHITE);

}

int main(int argc, char **argv)
{
	SDL_Surface *window, *surface, *starmap;
	SDL_Renderer* renderer;

	int quit = 0;
	SDL_Event e;
	int mPosX, mPosY, mPosPrecX, mPosPrecY;
	float mouseSensitivity = 1.2 * PI_2 / WIN_HEIGHT;
	int speed = 100, forward_mov = 0, lateral_mov = 0, upward_mov = 0;
	int goingToAstre = 0, move, moveMouse;
	float T0 = clock(), dT = 0;

	init_TUL();

	cam* c = createCam((point3) { -20000, 149597, 0 }, 0, 0, 4, 1.f / 150);

	point3 p0 = (point3){ 0, 0, 0 };
	point3 pX = (point3){ 10, 0, 0 };
	point3 pY = (point3){ 0, 10, 0 };
	point3 pZ = (point3){ 0, 0, 10 };

	point2 p02, pX2, pY2, pZ2;

	list* astres = NULL, *astreFocus = NULL;

	int nb_astres = 2;
	addAstre(&astres, createAstre((point3) { 0, 0, 0 }, (point3) { 0, 0, 0 }, (point3) { 0, 0, 0 }, 1.989, 30, 6.963, 3, 25, 0, 0, ETOILE, "Textures/sunmap.jpg", "Soleil"));
	addAstre(&astres, createAstre((point3) { 0, 149597, 0 }, (point3) { 0, 0, 0 }, (point3) { 0, 0, 0 }, 5.972, 24, 6.378, 3, 1, 0, -PI / 3.f, PLANETE, "Textures/earthmap.jpg", "Terre"));

	// Initialisation des fenêtres
	{
		if (SDL_Init(SDL_INIT_VIDEO) < 0) {
			fprintf(stderr, "ERREUR - SDL_Init\n>>> %s\n", SDL_GetError());
			exit(EXIT_FAILURE);
		}

		if (!(starmap = IMG_Load("Textures/starmap_2020_4k.jpg"))) {
			printf("Unable to open jpg file.\n");
			exit(EXIT_FAILURE);
		}

		atexit(SDL_Quit);
		window = SDL_CreateWindow("SDL2 Window", 0, SDL_WINDOWPOS_UNDEFINED, WIN_WIDTH, WIN_HEIGHT, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);

		if (window == NULL) {
			fprintf(stderr, "ERREUR - impossible de passer en : %dx%dx%d\n>>> %s\n", WIN_WIDTH, WIN_HEIGHT, 32, SDL_GetError());
			exit(EXIT_FAILURE);
		}

		surface = SDL_CreateRGBSurface(0, WIN_WIDTH, WIN_HEIGHT, 32, 0x00FF0000, 0x0000FF00, 0x000000FF, 0xFF000000);
		renderer = SDL_CreateRenderer(window, 0, 0);

		//mapPxls(surface, starmap);
		//renderSurface(renderer, surface);
	}

	SDL_GetMouseState(&mPosPrecX, &mPosPrecY);
	
	SDL_Keycode key;
	while (!quit)
	{
		dT = (clock() - T0) / (float)(1000 * 60); // 1 jour = 1 minute
		T0 = clock();
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

				// Manual displacements
				{
				case SDLK_z:
					forward_mov = 1;
					break;
				case SDLK_s:
					forward_mov = -1;
					break;
				case SDLK_q:
					lateral_mov = -2;
					break;
				case SDLK_d:
					lateral_mov = 2;
					break;
				case SDLK_LSHIFT:
					upward_mov = -2;
					break;
				case SDLK_SPACE:
					upward_mov = 2;
					break;
				}
				case SDLK_LEFT:
					if (!astreFocus) astreFocus = astres;
					else if (!astreFocus->prec) while (astreFocus->next) astreFocus = astreFocus->next;
					else astreFocus = astreFocus->prec;
					goingToAstre = 1;
					break;
				case SDLK_RIGHT:
					if (!astreFocus || !astreFocus->next) astreFocus = astres;
					else astreFocus = astreFocus->next;
					goingToAstre = 1;
					break;

				case SDLK_r:
					rotateCam(c, -c->lat, -c->lon);
					SDL_GetMouseState(&mPosPrecX, &mPosPrecY);
					break;
				}
				break;
			case SDL_MOUSEBUTTONDOWN:
				SDL_GetMouseState(&mPosX, &mPosY);
				break;
			case SDL_KEYUP:
				key = e.key.keysym.sym;

				if (key == SDLK_z || key == SDLK_s) forward_mov = 0;
				if (key == SDLK_q || key == SDLK_d) lateral_mov = 0;
				if (key == SDLK_LSHIFT || key == SDLK_SPACE) upward_mov = 0;
			}
		}

		SDL_GetMouseState(&mPosX, &mPosY);
		moveMouse = mPosX - mPosPrecX != 0 || mPosY - mPosPrecY != 0;
		move = forward_mov || lateral_mov || upward_mov;
		if (moveMouse || move || goingToAstre)
		{

			if (move) {
				float n_x = c->normale.x, n_y = c->normale.y, n_dir = sqrtf(n_x * n_x + n_y * n_y);
				float x_dir = n_x / n_dir, y_dir = n_y / n_dir;

				moveCam(c, scale3((point3) { forward_mov * x_dir - lateral_mov * y_dir, forward_mov * y_dir + lateral_mov * x_dir, upward_mov }, speed));
				//printf("[ %.2f ; %.2f ] et [ %.2f ; %.2f ; %.2f ]\n", c->lon, c->lat, c->pos.x, c->pos.y, c->pos.z);
				goingToAstre = 0;
			}
			
			if (moveMouse) {
				rotateCam(c, -(mPosY - mPosPrecY) * mouseSensitivity, (mPosX - mPosPrecX)* mouseSensitivity);
				//printf("[ %.2f ; %.2f ; %.2f ]\n", c->normale.x, c->normale.y, c->normale.z);
				mPosPrecX = mPosX;
				mPosPrecY = mPosY;
				goingToAstre = 0;
			}
			
			//rotateCam(c, -c->lat, -c->lon);


			if (goingToAstre) {
				goingToAstre = !goToAstre(c, astreFocus->p_astre);
			}

			//p02 = projectPoint(c, p0, WIN_WIDTH, WIN_HEIGHT);
			//pX2 = projectPoint(c, pX, WIN_WIDTH, WIN_HEIGHT);
			//pY2 = projectPoint(c, pY, WIN_WIDTH, WIN_HEIGHT);
			//pZ2 = projectPoint(c, pZ, WIN_WIDTH, WIN_HEIGHT);
			//drawshit(surface, c);

			//printf("[ %.2f ; %.2f ] et [ %.2f ; %.2f ; %.2f ]\n", c->lon, c->lat, PI2, PI, PI_2);
			//printf("[ %.2f ; %.2f ] | [ %.2f ; %.2f] | [ %.2f ; %.2f ]\n", pX2.x, pX2.y, pY2.x, pY2.y, pZ2.x, pZ2.y);


			//drawLine(surface, p02, pX2, RED);
			//drawLine(surface, p02, pY2, BLUE);
			//drawLine(surface, p02, pZ2, GREEN);
			//drawLine(surface, (point2){700, 700}, (point2) {430, 360}, WHITE);

		}

		mapBackground(surface, starmap, c, 0.25);

		renderAstres(surface, c, astres, nb_astres, dT);

		renderSurface(renderer, surface);

	}

	SDL_FreeSurface(starmap);
	SDL_FreeSurface(surface);
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);

	freeList(astres);

	return EXIT_SUCCESS;
}