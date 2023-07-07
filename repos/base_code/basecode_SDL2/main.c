#include <stdio.h>
#include <stdbool.h>
#include "SDL.h"
#include "utils.h"
#include "fonc.h"

enum ePosition
{
	DOWN,
	LEFT,
	UP,
	RIGHT,
	NB_POSITION
};

int main(int argc, char** argv)
{

	int gameTree[8][8];

	initGame(gameTree);

	tree* root = createTree(-1);

	//(root)->val = initTreeIA(gameTree, &root, 2, 1);

	int game[8][8];

	//Init game
	initGame(game);

	

	//Partie en cours
	


	SDL_Window* window;
	SDL_Renderer* renderer;

	SDL_Texture* texture;
	SDL_Surface* image;
	SDL_Rect spriteRect;
	SDL_Rect positionRect;

	SDL_Rect emplacement;
	SDL_Texture* texture_noir;
	SDL_Surface* image_noir;

	SDL_Rect emplacement_blanc;
	SDL_Texture* texture_blanc;
	SDL_Surface* image_blanc;

	SDL_Rect allNoir[8][8];
	SDL_Rect allBlanc[8][8];
	

	SDL_Event e;
	int quit = 0;

	int pos = DOWN;
	
	if (!SDL_WasInit(SDL_INIT_VIDEO))
	{
		if (SDL_Init(SDL_INIT_VIDEO) != 0)
		{
			printf("[-] ERROR - Failed to initialise SDL (%s)\n", SDL_GetError());
			return EXIT_FAILURE;
		}
	}

	if (!IMG_Init(IMG_INIT_PNG))
	{
		printf("[-] ERROR - Failed to initialise SDL_Image (%s)\n", SDL_GetError());
		return EXIT_FAILURE;
	}

	window = SDL_CreateWindow("Othello", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 640, 480, SDL_WINDOW_SHOWN);
	if (!window)
	{
		printf("[-] ERROR - Failed to create SDL window (%s)\n", SDL_GetError());
		return EXIT_FAILURE;
	}

	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
	
	image = IMG_Load("./data/othellier_blanc_modif.png");
	texture = SDL_CreateTextureFromSurface(renderer, image);
	SDL_FreeSurface(image);

	positionRect.x = 0;
	positionRect.y = 0;
	positionRect.h = 480;
	positionRect.w = 480;

	spriteRect.x = 0;
	spriteRect.y = 0;
	spriteRect.h = 224;
	spriteRect.w = 225;

	image_noir = IMG_Load("./data/rond_noir.png");
	texture_noir = SDL_CreateTextureFromSurface(renderer, image_noir);
	SDL_FreeSurface(image_noir);
	
	emplacement.x = 0;
	emplacement.y = 0;
	emplacement.h = 225;
	emplacement.w = 225;

	image_blanc = IMG_Load("./data/rond_blanc.png");
	texture_blanc = SDL_CreateTextureFromSurface(renderer, image_blanc);
	SDL_FreeSurface(image_blanc);

	emplacement_blanc.x = 0;
	emplacement_blanc.y = 0;
	emplacement_blanc.h = 1300;
	emplacement_blanc.w = 1300;


	int scoreJ = 0, scoreIA = 0;

	SDL_SetRenderDrawColor(renderer,0, 255, 255, 255);
	int x=0;
	int y=0;
	bool turn=false;

	
	while (!quit)
	{
		
		while (SDL_PollEvent(&e))
		{
			switch (e.type)
			{
			case SDL_QUIT:
				quit = 1;
				break;

			/*case SDL_MOUSEMOTION:
				x = e.motion.x;
				y = e.motion.y;
				printf("%d %d\n", x, y);
				break;*/
			case SDL_MOUSEBUTTONDOWN:
				x = e.button.x;
				y = e.button.y;
				
				if (x >= 45 && y >= 49 && x <= 456 && y <= 461) {
					int col = (int) (x / 51)-1 ;
					int lin = (int) (y / 51) -1;
					
					/*intf("Au tour des Noirs\n");
					if (turn == false){
						
						turnJoueur(game, lin, col, 1);
						if (game[lin][col] == 1)
							turn = true;						
						printGame(game);
						break;
						
					}
					printf("Au tour des Blancs\n");
					if (turn == true) {
						(root)->val = initTreeIA(gameTree, &root, 2, -1);
						printTree(root);
						turnIA(game, root);
						
						turn = false;
						printGame(game);
						break;
					}*/
					if (turn == false) {
						turnJoueur(game, lin, col, 1);
						if (game[lin][col] == 1)
							turn = true;
						printGame(game);
					}
					
					

					
					printf("Au tour des Blancs\n");
					tabCpy(gameTree, game);
					(root)->val = initTreeIA(gameTree, &root, 3, -1);
					printTree(root);
					turnIA(game, root);

					turn = false;
					printGame(game);
					

					
					
				}
				
				break;

			case SDL_KEYDOWN:
				switch (e.key.keysym.sym)
				{

					case SDLK_ESCAPE:
						quit = 1;
						break;
					default:
						break;
				}
			default:
				break;
			}
			
		}
		for (int i = 0; i < 8; i++) {
			for (int j = 0; j < 8; j++) {
				if (game[i][j] == 1) {
					allNoir[i][j].x = (j + 1) * 51;
					allNoir[i][j].y = (i + 1) * 52;
					allNoir[i][j].h = 45;
					allNoir[i][j].w = 45;


				}
				if (game[i][j] == -1) {
					allBlanc[i][j].x = (j + 1) * 51;
					allBlanc[i][j].y = (i + 1) * 52;
					allBlanc[i][j].h = 45;
					allBlanc[i][j].w = 45;


				}
			}
		}

		SDL_RenderClear(renderer);
		
		SDL_RenderCopy(renderer, texture, &spriteRect, &positionRect);
		for (int i = 0; i < 8; i++) {
			for (int j = 0; j < 8; j++) {
				if(game[i][j]==1)
					SDL_RenderCopy(renderer, texture_noir, &emplacement, &allNoir[i][j]);
				if (game[i][j] == -1)
					SDL_RenderCopy(renderer, texture_blanc, &emplacement_blanc, &allBlanc[i][j]);
				
			}
		}
		
		
		SDL_RenderPresent(renderer);
	}

	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();

	return EXIT_SUCCESS;
}

