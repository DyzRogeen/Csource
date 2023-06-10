#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include "utils_SDL.h"
#include "gravity.h"

int main(int argc, char **argv)
{
	point pos, dir;
	float mass, radius;
	char* name = (char*)calloc(10,sizeof(char));
	SDL_Surface * window;
	int quit = 0;
	SDL_Event e;
	entity* eList[10];
	entity* ent;
	int nbEntity = 0;
	int pause = 0;
	int trace = 0;
	int deltaT = 20;
	int tickT = 0;

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

	// On met le titre sur la fenêtre
	SDL_WM_SetCaption("VonKoch", NULL);

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
				if (e.key.keysym.sym == SDLK_a && nbEntity > 0)
					nbEntity--;
				if (e.key.keysym.sym == SDLK_LEFT)
					printf("DeltaT = %d\n", ++deltaT);
				if (e.key.keysym.sym == SDLK_RIGHT)
					printf("DeltaT = %d\n", --deltaT);
				if (e.key.keysym.sym == SDLK_t)
					trace = trace ? 0 : 1;
				if (e.key.keysym.sym == SDLK_p) {
					pause = pause ? 0 : 1;
					if (pause)
						printf("PAUSED\n");
					else
						printf("UNPAUSED\n");
				}
					
				if (e.key.keysym.sym == SDLK_q || e.key.keysym.sym == SDLK_e) {

					if (e.key.keysym.sym == SDLK_e) {
						printf("QUICK ADD ENTITY FORMAT : X_POS Y_POS MASS RADIUS\n");
						scanf_s("%f %f %f %f", &pos.x, &pos.y, &mass, &radius);
						name = (char*)nbEntity;
						dir.x = 0.f;
						dir.y = 0.f;
					}
					else {
						printf("ADD ENTITY FORMAT : X_POS Y_POS X_DIR Y_DIR MASS RADIUS NAME\n");
						scanf_s("%f %f %f %f %f %f %s", &pos.x, &pos.y, &dir.x, &dir.y, &mass, &radius, name);
					}

					ent = CreateEntity(pos, dir, mass, radius, name);
					eList[nbEntity++] = ent;
				}

				break;
			}
		}

		if (!pause && tickT >= deltaT*1000) {

			// On vide la fenêtre (on la remplit en noir)
			SDL_FillRect(window, &(window->clip_rect), SDL_MapRGB(window->format, 0, 0, 0));

			SimulOnce(window, eList, trace, nbEntity);

			// Met à jour le buffer de la fenêtre
			SDL_UpdateRect(window, 0, 0, 0, 0);
			// Flip le buffer pour l'envoyer vers l'écran (la fenêtre est configurée en double buffer)
			SDL_Flip(window);

			tickT = 0;

		}
		if (!pause) tickT++;
	}

	return EXIT_SUCCESS;
}