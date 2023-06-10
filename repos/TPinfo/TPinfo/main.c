#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "fonc.h"
#include "SDL.h"


int main(int argv, char** argc) {

	int game[8][8];
	
	//Init game
	for (int i = 0; i < 8; i++) {
		for (int j = 0; j < 8; j++) {
			game[i][j] = 0;
		}
	}
	game[3][3] = -1;
	game[4][4] = -1;
	game[3][4] = 1;
	game[4][3] = 1;

	//Affichage du plateau
	printGame(game);

	SDL_Window * window=NULL;
	SDL_Event evt;
    SDL_Renderer * renderer=NULL;
	if (SDL_Init(SDL_INIT_VIDEO) < 0) {
		fprintf(stderr, "ERREUR - SDL_Init\n>>> %s\n", SDL_GetError());
		exit(EXIT_FAILURE);
	}
    window = SDL_CreateWindow("Fenetre", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 800, 600, 0);
    if (window == NULL)
        SDL_ExitWithError("Creation fenetre échoué");

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_SOFTWARE);
    if(renderer==NULL)
        SDL_ExitWithError("Creation rendu échoué");
    SDL_RenderPresent(renderer);

	
    SDL_SetWindowTitle(window, "    Reversi");
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);

	
    atexit(SDL_Quit);
    bool quit = false;
    while (!quit)
    {
        SDL_Scancode scanCode;

        while (SDL_PollEvent(&evt))
        {
            switch (evt.type)
            {
            case SDL_QUIT:
                quit = true;
                break;

            case SDL_KEYDOWN:
                scanCode = evt.key.keysym.scancode;
                if (evt.key.repeat)
                    break;

                switch (scanCode)
                {
                case SDL_SCANCODE_ESCAPE:
                    quit = true;
                    break;

                default:
                    break;

                }
            default:
                break;
            }
        }

    }
    SDL_RenderDrawLine(renderer, 0, 0, 1000, 1000);
    

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}