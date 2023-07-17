#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include "utils_SDL.h"
#include "mot.h"

void updateScreen(SDL_Surface * window, int* Z_Buffers, listO* lo, int nbObj, cam C, light L, int camHasMoved, int* alt) {

	// On vide la fenêtre (on la remplit en noir)
	SDL_FillRect(window, &(window->clip_rect), SDL_MapRGB(window->format, 0, 0, 0));

	for (int i = 0; i < nbObj; i++) {
		displayObj(window, Z_Buffers, *lo->o, C, L, camHasMoved, *alt);
		lo = lo->next;
	}

	// Met à jour le buffer de la fenêtre
	SDL_UpdateRect(window, 0, 0, 0, 0);
	// Flip le buffer pour l'envoyer vers l'écran (la fenêtre est configurée en double buffer)
	SDL_Flip(window);

	*alt *= -1;
}

int main(int argc, char** argv)
{

	SDL_Surface* window;
	SDL_Event e;

	int quit, dirx, diry, dirz, roty;
	quit = dirx = diry = dirz = roty = 0;

	int alt = 1;

	point mPos, premPos;
	SDL_GetMouseState(&premPos.x, &premPos.y);
	float speed = 0.4;
	float jsp = 10;

	cam C = initCam(setPoint(0, 300, 0), setPoint(jsp, 0, 0));
	light L = initLight(setPoint(0, 500, 300), 10, (Uint8[3]){255, 255, 255});
	listO* O = (listO*)calloc(1, sizeof(listO));
	int nbObj = 0;

	addObj(&O, createCube(setPoint(400, 0, 0), setPoint(0, 0, 0), 400, (Uint8[3]) {120, 120, 120}, 1));
	addObj(&O, createCube(setPoint(400, 0, 800), setPoint(PI / 4, 0, PI / 4), 300, (Uint8[3]) {200, 0, 0}, 0));
	addObj(&O, createCube(setPoint(400, 0, -800), setPoint(0, 0, 0), 300, (Uint8[3]) { 0, 200, 0 }, 0));
	ExtrudeFace(O->o, setPoint(-200, 100, 100));

	nbObj = 3;

	// Initialisation
	if (SDL_Init(SDL_INIT_VIDEO) < 0) {
		fprintf(stderr, "ERREUR - SDL_Init\n>>> %s\n", SDL_GetError());
		exit(EXIT_FAILURE);
	}

	// Permet de quitter proprement par la suite (revenir à la résolution initiale de l'écran avec le bon format vidéo)
	atexit(SDL_Quit);
	// Initialise le mode vidéo de la fenêtre
	window = SDL_SetVideoMode(640, 480, 32, SDL_HWSURFACE | SDL_DOUBLEBUF);
	// Si on n'a pas réussi à créer la fenêtre on retourne une erreur
	if (window == NULL) {
		fprintf(stderr, "ERREUR - impossible de passer en : %dx%dx%d\n>>> %s\n", 640, 480, 32, SDL_GetError());
		exit(EXIT_FAILURE);
	}

	int* Z_Buffers = (int*)calloc(window->h * window->w, sizeof(int));

	// On met le titre sur la fenêtre
	SDL_WM_SetCaption("VonKoch", NULL);

	updateScreen(window, Z_Buffers, O, nbObj, C, L, 1, &alt);

	while (!quit)
	{
		while (SDL_PollEvent(&e))
		{
			switch (e.type)
			{
			case SDL_QUIT:
				quit = 1;
				freeAll(O);
				break;
			case SDL_KEYDOWN:
				if (e.key.keysym.sym == SDLK_ESCAPE)
					quit = 1;
				if (e.key.keysym.sym == SDLK_w) dirx = 1;
				if (e.key.keysym.sym == SDLK_s) dirx = -1;
				if (e.key.keysym.sym == SDLK_d) diry = 1;
				if (e.key.keysym.sym == SDLK_a) diry = -1;
				if (e.key.keysym.sym == SDLK_z) dirz = 1;
				if (e.key.keysym.sym == SDLK_x) dirz = -1;
				if (e.key.keysym.sym == SDLK_r) C = initCam(setPoint(0,300,0),setPoint(jsp,0,0));
				if (e.key.keysym.sym == SDLK_e) jsp /= 10;
				if (e.key.keysym.sym == SDLK_q) jsp *= 10;
				if (e.key.keysym.sym == SDLK_p) roty = 1;

				break;
			case SDL_KEYUP:
				if (e.key.keysym.sym == SDLK_w || e.key.keysym.sym == SDLK_s) dirx = 0;
				if (e.key.keysym.sym == SDLK_a || e.key.keysym.sym == SDLK_d) diry = 0;
				if (e.key.keysym.sym == SDLK_z || e.key.keysym.sym == SDLK_x) dirz = 0;
				if (e.key.keysym.sym == SDLK_p) roty = 0;

			}
		}

		SDL_GetMouseState(&mPos.x, &mPos.y);
		if (mPos.x - premPos.x || mPos.y - premPos.y) {
			rotateCam(&C, (mPos.x - premPos.x) / 100.0, (mPos.y - premPos.y) / -100.0);
			updateScreen(window, Z_Buffers, O, nbObj, C, L, 0, &alt);

		}

		if (dirx || diry || dirz) {
			moveCam(&C,setPoint((dirx * C.dir.x - diry * C.dir.z)*speed/jsp, dirz, (dirx * C.dir.z + diry * C.dir.x)*speed/jsp));
			updateScreen(window, Z_Buffers, O, nbObj, C, L, 1, &alt);
		}

		if (roty) {
			RotateObj(O->o, setPoint(0.01, 0, 0));
			RotateObj(O->next->o, setPoint(0, 0, 0.01));
			RotateObj(O->next->next->o, setPoint(0, 0.01, 0));
			updateScreen(window, Z_Buffers, O, nbObj, C, L, 1, &alt);
		}

		premPos.x = mPos.x;
		premPos.y = mPos.y;

	}
	free(Z_Buffers);
	return EXIT_SUCCESS;
}