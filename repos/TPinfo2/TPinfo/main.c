#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "fonc.h"

int main(int argv, char** argc) {

	int gameTree[8][8];

	tree* root = createTree(-1);

	//initTreeIA(gameTree, &root, 4, 1);
	
	int game[8][8];
	
	//Init game
	for (int i = 0; i < 8; i++) {
		for (int j = 0; j < 8; j++) {
			game[i][j] = 0;
		}
	}
	game[3][3] = 1;
	game[4][4] = 1;
	game[3][4] = -1;
	game[4][3] = -1;

	int scoreJ = 0, scoreIA = 0;

	//Affichage du plateau
	//printGame(game);

	bool partie = true;

	//Partie en cours
	while (partie) {

		printGame(game);

		if (victory(game) == 0) {

			turnJoueur(game, 1);
			printGame(game);

		}
		
		if (victory(game) == 0)
			turnJoueur(game, -1);

		else {

			int res = victory(game);
			if (res > 0) {

				scoreJ = res;
				scoreIA = 64 - res;

			}
			if (res < 0) {

				scoreIA = res;
				scoreJ = 64 - res;

			}
			printf("FIN DE PARTIE !\nScore : Joueur : %d - IA : %d\n", scoreJ, scoreIA);

			partie = false;

		}

	}

}