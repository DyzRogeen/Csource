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

	SDL_Rect emplacement_rouge;
	SDL_Texture* texture_rouge;
	SDL_Surface* image_rouge;

	SDL_Rect allNoir[8][8];
	SDL_Rect allBlanc[8][8];
	SDL_Rect casePossible[8][8];
	
	SDL_Event e;

	Mix_Music* musique;

	
	//Initialisation message score
	
	SDL_Surface* message = NULL;
	SDL_Texture* texture_test = NULL;
	TTF_Font* font= NULL;
	SDL_Color textColor = { 0, 0, 0 };

	SDL_Rect emplacement_score;
	
	
	//Initialisation message score
	SDL_Surface* score = NULL;
	SDL_Texture* texture_score = NULL;
	TTF_Font* font_score = NULL;
	SDL_Color textColor_score = { 0, 0, 0 };
	SDL_Rect emplacement_nb;

	//Initialisation Victoire
	SDL_Surface* victoire = NULL;
	SDL_Texture* texture_victoire = NULL;
	TTF_Font* font_victoire = NULL;
	SDL_Color textColor_victoire = { 48, 200, 36};
	SDL_Rect emplacement_victoire;

	//Initialisation défaite
	SDL_Surface* defaite = NULL;
	SDL_Texture* texture_defaite = NULL;
	TTF_Font* font_defaite = NULL;
	SDL_Color textColor_defaite = { 255, 0, 0 };
	SDL_Rect emplacement_defaite;

	//Initialisation égalité
	SDL_Surface* egalite = NULL;
	SDL_Texture* texture_egalite = NULL;
	TTF_Font* font_egalite = NULL;
	SDL_Color textColor_egalite = { 195, 195, 195 };
	SDL_Rect emplacement_egalite;


	int quit = 0;

	int pos = DOWN;
	
	if (!SDL_WasInit(SDL_INIT_EVERYTHING))
	{
		if (SDL_Init(SDL_INIT_EVERYTHING) != 0)
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
	
	if (TTF_Init())
	{
		
		printf("Erreur de TTF (%s)\n", TTF_GetError());
		return EXIT_FAILURE;
	}
	
	if (!Mix_Init(MIX_INIT_MP3)) //Initialisation de l'API Mixer
	{
		printf("%s", Mix_GetError());
		return EXIT_FAILURE;
	}
	
	
	

	window = SDL_CreateWindow("Othello", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 740, 480, SDL_WINDOW_SHOWN);
	if (!window)
	{
		printf("[-] ERROR - Failed to create SDL window (%s)\n", SDL_GetError());
		return EXIT_FAILURE;
	}

	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
	
	//****************************************************
	//Load l'image/texture de l'othellierr
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

	//****************************************************
	//Load l'image/texture du pion blanc
	image_noir = IMG_Load("./data/rond_noir.png");
	texture_noir = SDL_CreateTextureFromSurface(renderer, image_noir);
	SDL_FreeSurface(image_noir);
	
	emplacement.x = 0;
	emplacement.y = 0;
	emplacement.h = 225;
	emplacement.w = 225;

	//****************************************************
	//Load l'image/texture du pion blanc
	image_blanc = IMG_Load("./data/rond_blanc.png");
	texture_blanc = SDL_CreateTextureFromSurface(renderer, image_blanc);
	SDL_FreeSurface(image_blanc);

	emplacement_blanc.x = 0;
	emplacement_blanc.y = 0;
	emplacement_blanc.h = 1300;
	emplacement_blanc.w = 1300;

	//****************************************************
	//Load l'image/texture des cases possibles
	image_rouge = IMG_Load("./data/point_rouge_modif.png");
	texture_rouge = SDL_CreateTextureFromSurface(renderer, image_rouge);
	SDL_FreeSurface(image_rouge);

	emplacement_rouge.x = 0;
	emplacement_rouge.y = 0;
	emplacement_rouge.h = 225;
	emplacement_rouge.w = 225;

	//****************************************************
	//Load le Score
	font = TTF_OpenFont("./data/ARLRDBD.TTF", 72);
	
	message = TTF_RenderText_Solid(font,"SCORE :", textColor);
	
	texture_test = SDL_CreateTextureFromSurface(renderer, message);
	
		
	SDL_FreeSurface(message);

	emplacement_score.x = 500;
	emplacement_score.y = 50;
	emplacement_score.h =130;
	emplacement_score.w = 200;


	//****************************************************
	//Load le Score a-b
	 
	font_score = TTF_OpenFont("./data/ARLRDBD.TTF", 72);
	
	emplacement_nb.x = 493;
	emplacement_nb.y = 170;
	emplacement_nb.h = 130;
	emplacement_nb.w = 200;


	//*********************************
	//Load message victoire
	font_victoire = TTF_OpenFont("./data/ARLRDBD.TTF", 72);
	victoire = TTF_RenderText_Solid(font_victoire, "Victoire !", textColor_victoire);
	texture_victoire = SDL_CreateTextureFromSurface(renderer, victoire);

	SDL_FreeSurface(victoire);

	emplacement_victoire.x = 500;
	emplacement_victoire.y = 338;
	emplacement_victoire.h = 130;
	emplacement_victoire.w = 200;

	//*********************************
	//Load message defaite
	font_defaite = TTF_OpenFont("./data/ARLRDBD.TTF", 72);
	defaite = TTF_RenderText_Solid(font_defaite, "Défaite :( !", textColor_defaite);
	texture_defaite = SDL_CreateTextureFromSurface(renderer, defaite);

	SDL_FreeSurface(defaite);

	emplacement_defaite.x = 500;
	emplacement_defaite.y = 338;
	emplacement_defaite.h = 130;
	emplacement_defaite.w = 200;

	//*********************************
	//Load message egalite
	font_egalite = TTF_OpenFont("./data/ARLRDBD.TTF", 72);
	egalite = TTF_RenderText_Solid(font_egalite, "Egalite !", textColor_egalite);
	texture_egalite = SDL_CreateTextureFromSurface(renderer, egalite);

	SDL_FreeSurface(egalite);

	emplacement_egalite.x = 500;
	emplacement_egalite.y = 338;
	emplacement_egalite.h = 130;
	emplacement_egalite.w = 200;
	//****************************************************
	//Load la musique
	musique = Mix_LoadMUS("bruit_pion.mp3");
	Mix_VolumeMusic(MIX_MAX_VOLUME );
	Mix_PlayMusic(musique, -1);
	if (Mix_PausedMusic() == 1)
		printf("Sa ne se jou pas");



	//****************************************************
	int scoreJ = 0, scoreIA = 0;

	SDL_SetRenderDrawColor(renderer,255, 255, 255, 255);
	int x=0;
	int y=0;
	bool turn=false;
	bool show = false;

	//****************************************************
	//Initialisation des 1er coups possibles
	int gamePossible[8][8];
	gamePossible[2][4] = -2;
	gamePossible[3][5] = -2;
	gamePossible[4][2] = -2;
	gamePossible[5][3] = -2;

	
	while (!quit)
	{
		while (SDL_PollEvent(&e))
		{
			switch (e.type)
			{
			case SDL_QUIT:
				quit = 1;
				break;

			case SDL_MOUSEBUTTONDOWN:
				x = e.button.x;
				y = e.button.y;
				printf("%d %d\n", x, y);
				if (x >= 45 && y >= 49 && x <= 456 && y <= 461) {

					//Enregistrement des colones et lignes du tableau ou on a cliqué
					int col = (int)(x / 51) - 1;
					int lin = (int)(y / 51) - 1;
					
					//1er joueur : noir
					if (turn == false && game[lin][col] == 0 ) {

						
						turnJoueur(game, lin, col, 1);
						
						if (game[lin][col] == 1) {
							
							Mix_PlayMusic(musique, 1);
							if (Mix_PlayMusic(musique, 1) == 1)
								printf("On chante");
							turn = true;
							for (int i = 0; i < 8; i++) {
								for (int j = 0; j < 8; j++) {
									gamePossible[i][j] = 0;
								}
							}
							break;
						}
						
					}

					//2ème joueur non IA : blanc
					/*
					if (turn == true && (game[lin][col] == 0 || game[lin][col] == 10)) {
						turnJoueur(game, lin, col, -1);

						if (game[lin][col] == -1) {
							Mix_PlayMusic(musique, 1);
							for (int i = 0; i < 8; i++) {
								for (int j = 0; j < 8; j++) {
									if (verify(game, i, j, 1, 0) && game[i][j] == 0)
										gamePossible[i][j] = -2;
								}
							}
							turn = false;
							break;
						}
					}*/

					//2ème joueur IA : blanc
					
					
				}

			case SDL_KEYDOWN:
				switch (e.key.keysym.sym)
				{
				case SDLK_ESCAPE:
					quit = 1;
					break;
				default:
					break;
				}
				switch (e.key.keysym.scancode)
				{

				case SDL_SCANCODE_H:
					if (show == true)
						show = false;
					else
						show = true;
					break;
				default:
					break;
				}
			default:
				break;
			}

		}
		

		//mettre les emplacements des pions dans un tableau
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
				if (gamePossible[i][j] == -2) {
					casePossible[i][j].x = (j + 1) * 51;
					casePossible[i][j].y = (i + 1) * 52;
					casePossible[i][j].h = 40;
					casePossible[i][j].w = 40;
				}
			}
		}
		if (turn == true) {
			tabCpy(gameTree, game);
			(root)->val = initTreeIA(gameTree, &root, 2, 2, -1);
			printTree(root);

			turnIA(game, root);

			freeTree(&root, 0);

			if (game[root->strike[0]][root->strike[1]] == -1) {
				Mix_PlayMusic(musique, 1);
				for (int i = 0; i < 8; i++) {
					for (int j = 0; j < 8; j++) {
						if (verify(game, i, j, 1, 0) && game[i][j] == 0)
							gamePossible[i][j] = -2;
					}
				}

				turn = false;
				printGame(game);
			}

		}
		SDL_RenderClear(renderer);

		//affichage othellier
		SDL_RenderCopy(renderer, texture, &spriteRect, &positionRect);

		//Affichage pion
		for (int i = 0; i < 8; i++) {
			for (int j = 0; j < 8; j++) {
				if (game[i][j] == 1)
					SDL_RenderCopy(renderer, texture_noir, &emplacement, &allNoir[i][j]);
				if (game[i][j] == -1)
					SDL_RenderCopy(renderer, texture_blanc, &emplacement_blanc, &allBlanc[i][j]);
				if (gamePossible[i][j] == -2 && show == true) {
					SDL_RenderCopy(renderer, texture_rouge, &emplacement_rouge, &casePossible[i][j]);
				}
			}
		}
		//Affichage message score

		SDL_RenderCopy(renderer, texture_test, NULL, &emplacement_score);

		//*****************************************
		//Affichage score
		int scoreJ = countCurrent(game);
		char scoreJ_mod = '0';
		if (scoreJ > 9) {
			scoreJ %= 10;
			scoreJ_mod = ((countCurrent(game) - scoreJ) / 10) + '0';
		}
		char scoreJ_c = scoreJ + '0';

		int scoreIA = countRound(game) - countCurrent(game);
		char scoreIA_mod = '0';
		int scoreIA_unit = scoreIA%10;
		scoreIA_mod = (scoreIA - scoreIA_unit) / 10 + '0';
		char separe = '-';

		char vs[100] = { scoreJ_mod,scoreJ_c,' ', ' ',  separe,' ', ' ' ,  scoreIA_mod, scoreIA_unit + '0'};

		score = TTF_RenderText_Solid(font_score,vs, textColor_score);
		texture_score = SDL_CreateTextureFromSurface(renderer, score);
		
		SDL_FreeSurface(score);
		SDL_RenderCopy(renderer, texture_score, NULL, &emplacement_nb);
		SDL_DestroyTexture(texture_score);

		
		
		if (victory(game) == 1) {
			int J = countCurrent(game);
			int IA = countRound(game) - countCurrent(game);
			if(J>IA)
				SDL_RenderCopy(renderer, texture_victoire, NULL, &emplacement_victoire);
			if(J==IA)
				SDL_RenderCopy(renderer, texture_egalite, NULL, &emplacement_egalite);
			if(J<IA)
				SDL_RenderCopy(renderer, texture_defaite, NULL, &emplacement_defaite);
			//break;
		}

		//*****************************************
		SDL_RenderPresent(renderer);
		//*****************************************

		
	}

	
	Mix_FreeMusic(musique);
	TTF_CloseFont(font);
	TTF_CloseFont(font_score);
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	
	TTF_Quit();
	Mix_Quit();
	IMG_Quit();
	SDL_Quit();

	return EXIT_SUCCESS;
}

