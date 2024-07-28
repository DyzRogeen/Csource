#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include "utils_SDL.h"
#include "mot.h"

void updateScreen(SDL_Surface * window, int* Z_Buffers, listO* lo, listS* S, cam C, listL* L, int camHasMoved, int* alt) {

	// On vide la fenêtre (on la remplit en noir)
	SDL_FillRect(window, &(window->clip_rect), SDL_MapRGB(window->format, 0, 0, 0));
	//displayFloorLines(window, Z_Buffers, C);
	*((Uint32*)window->pixels + window->w / 2 + window->h * window->w / 2) = SDL_MapRGB(window->format, 200, 200, 200);

	while (lo) {
		displayObj(window, Z_Buffers, *lo->o, C, L, camHasMoved, *alt);
		lo = lo->next;
	}

	moveSpheres(S);
	drawSpheres(window, Z_Buffers, S, L, C, *alt);

	displayLights(window, Z_Buffers, L, C, *alt);
	
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

	int quit, dirx, diry, dirz, roty, dirx_z, diry_z;
	quit = dirx = diry = dirz = roty = 0;

	int alt = 1, updateS = 0;

	int mPosx, mPosy;
	point mPos, premPos;
	SDL_GetMouseState(&premPos.x, &premPos.y);
	float speed = 5;
	float jsp = 1, pct;

	cam C = initCam(setPoint(0, 300, 0), setPoint(jsp, 0, 0));

	listL* L = NULL;
	addLight(&L, createLight(createPoint(0, 500, 300), 10, (Uint32)(255 << 16 | 255 << 8 | 255)));

	listO* O = (listO*)calloc(1, sizeof(listO));
	O->next = NULL;
	O->o = createCube(setPoint(400, 1000, 800), setPoint(0, 0, 0), 400, (Uint32) (120 << 16 | 120 << 8 | 120), 1);
	addObj(&O, createCube(setPoint(400, 0, 800), setPoint(PI / 4, 0, PI / 4), 300, (Uint32) (200 << 16), 0));
	addObj(&O, createCube(setPoint(400, 0, -800), setPoint(0, 0, 0), 300, (Uint32) (200 << 8), 0));
	//ExtrudeFace(O->o, setPoint(-200, 100, 100));

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
	listS* S = createListS(createSphere(200, createPoint(400, 600, 0), SDL_MapRGB(window->format, 50, 50, 50)));

	// On met le titre sur la fenêtre
	SDL_WM_SetCaption("VonKoch", NULL);

	updateScreen(window, Z_Buffers, O, S, C, L, 1, &alt);

	while (!quit)
	{
		while (SDL_PollEvent(&e))
		{
			switch (e.type)
			{
			case SDL_QUIT:
				quit = 1;
				freeAll(O, S, L);
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

		SDL_GetMouseState(&mPosx, &mPosy);
		if (mPosx - premPos.x || mPosy - premPos.y) {
			rotateCam(&C, (mPosx - premPos.x) / 100.0, (mPosy - premPos.y) / -100.0);
			updateS = 1;
		}

		if (dirx || diry || dirz) {
			pct = fabs(C.dir.x) / (fabs(C.dir.x) + fabs(C.dir.z));
			moveCam(&C,setPoint((dirx * (C.dir.x + fabs(C.dir.y) * pct) - diry * (C.dir.z + fabs(C.dir.y) * (1 - pct))) * speed/jsp, dirz, (dirx * (C.dir.z + fabs(C.dir.y) * (1 - pct)) + diry * (C.dir.x + fabs(C.dir.y) * pct)) * speed/jsp));
			updateS = 1;
		}

		if (roty) {
			RotateObj(O->o, setPoint(0.01, 0, 0));
			RotateObj(O->next->o, setPoint(0, 0, 0.01));
			RotateObj(O->next->next->o, setPoint(0, 0.01, 0));
			updateS = 1;
		}

		if (updateS) {
			updateScreen(window, Z_Buffers, O, S, C, L, 1, &alt);
			updateS = 0;
		} else updateScreen(window, Z_Buffers, O, S, C, L, 0, &alt);

		premPos.x = mPosx;
		premPos.y = mPosy;

	}
	free(Z_Buffers);
	return EXIT_SUCCESS;
}