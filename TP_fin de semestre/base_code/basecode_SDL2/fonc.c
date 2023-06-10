#include "fonc.h"
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>


tree* createTree(int val) {

	tree* root = (tree*)calloc(1, sizeof(tree));
	root->val = val;
	return root;

}

void freeTree(tree** root, int prof) {

	if (!root) return;
	if (!(*root)) {
		printf("J'ai free\n");
		free(root);
		return;
	}

	for (int n = 0; n < (*root)->nb_child; n++) {

		if ((*root)->child[n])
			freeTree(&((*root)->child[n]), prof + 1);

	}

}

void printGame(int game[8][8]) {

	printf("[]===============================[]\\\n");

	for (int i = 0; i < 8; i++) {

		printf("||");

		for (int j = 0; j < 8; j++) {

			if (game[i][j] == 0) printf("   ");
			if (game[i][j] == 1) printf(" O ");
			if (game[i][j] == -1) printf(" @ ");
			printf("|");

		}

		printf("| |%d\n", i + 1);
		if (i != 7) printf("||---+---+---+---+---+---+---+---|| |\n");

	}

	printf("[]===============================[] |\n");
	printf("\\__________________________________\\|\n");
	printf("   A   B   C   D   E   F   G   H\n");

}

void turnJoueur(int game[8][8],int lin, int col,  int turn) {

	

	bool choix_De_Coup = true;

	//Demande de Placement
	while (choix_De_Coup) {

		if (victory(game)) {
			game[lin][col] = 10;
			return;
		}

			 
		if (0 > col || 0 > lin || 7 < col || 7 < lin || game[lin][col] != 0) {

			//printf("ERREUR : Coup non valide !\n");
			return;
		}
		else if (CaseJouable(game) == 0) {
			printf("tu 'as pas de case jouable");
			return;
		}
			

		else if (verify(game, lin, col, turn, 1)) {
			choix_De_Coup = false;
			return;
		}
		else {
			//printf("ERREUR : Coup non efficace !\n");
			return;
		}

	}

}

void turnIA(int game[8][8], tree* root) {

	if (victory(game) != 0) {

		printf("L IA A PAS DE CASE JOUABLE\n");
		return;

	}

	int i = root->strike[0];
	int j = root->strike[1];

	verify(game, i, j, -1, 1);
	printf("Le coup joue est %d %d \n", i, j);
}


int verify(int game[8][8], int col, int lin, int turn, int modify) {

	int a = 1;
	bool Ally_Found = false;
	bool coup_Eff = false;

	for (int i = -1; i <= 1; i++) {

		for (int j = -1; j <= 1; j++) {

			if (i == 0 && j == 0) continue;

			if (game[col + i][lin + j] == turn * -1) {

				a = 2;
				Ally_Found = false;

				//CONDITIONS A VERIFIER
				while (a != 1 && 0 <= (col + a * i) && 0 <= (lin + a * j) && 8 > (col + a * i) && 8 > (lin + a * j) && game[col + a * i][lin + a * j] != 0) {

					//Si on trouve un pion allié
					if (game[col + a * i][lin + a * j] == turn) {

						Ally_Found = true;
						coup_Eff = true;
					}
					if (Ally_Found) {

						a--;
						if (modify) game[col + a * i][lin + a * j] = turn;

					}
					else a++;
				}
			}
		}
	}

	if (coup_Eff) {

		if (modify) game[col][lin] = turn;
		return 1;
	}
	else return 0;
}

int victory(int game[8][8]) {

	for (int i = 0; i < 8; i++) {
		for (int j = 0; j < 8; j++) {

			if (game[i][j] == 0 && verify(game, i, j, 1, 0))
				return 0;

		}
	}

	for (int i = 0; i < 8; i++) {
		for (int j = 0; j < 8; j++) {

			if (game[i][j] == 0 && verify(game, i, j, -1, 0))
				return 0;

		}
	}
	
	return 1;

}


int initTreeIA(int game[8][8], tree** root, int prof, int profStart, int turn) {

	int res = -1;
	int min = 64;
	int max = 0;
	int n = 0;

	//(*root)->child = (tree**)calloc(1, sizeof(tree*));

	if (prof == 0 || victory(game) != 0) {

		res = countRound(game) - countCurrent(game);
		(*root)->val = res;
		return res;

	}

	//Sauvegarde du plateau propre à la racine
	int gameSave[8][8];

	tabCpy(gameSave, game);

	//Enumère les possibilités du jeu actuel
	for (int i = 0; i < 8; i++) {
		for (int j = 0; j < 8; j++) {

			//printf("Je verifie (%d, %d)...\n", i, j);

			if (game[i][j] == 0) {

				if (verify(game, i, j, turn, 1)) {

					//printf("Prof : %d ; (%d, %d) est une case jouable\n", prof, i, j);

					(*root)->child[n] = (tree*)calloc(1, sizeof(tree));
					(*root)->nb_child = n + 1;

					(*root)->child[n]->strike[0] = i;
					(*root)->child[n]->strike[1] = j;

					res = initTreeIA(game, (*root)->child + n, prof - 1, profStart, turn * -1);

					(*root)->child[n]->val = res;

					//printf("Prof : %d ; %d retournee\n", prof, res);

					if (prof % 2 == profStart % 2) {

						if (res > max) {

							max = res;
							
							
							//Elagage Max
							for (int k = n - 1; k >= 0; k--) {

								if ((*root)->child[n] <= (*root)->child[k])
									return res;

							}
						
							if (prof == profStart) {

								(*root)->strike[0] = i;
								(*root)->strike[1] = j;

							}

						}

					}

					else {

						if (res < min) {

							min = res;

							
							//Elagage Min
							for (int k = n - 1; k >= 0; k--) {

								if ((*root)->child[n] >= (*root)->child[k])
									return res;

							}

						}

					}

					tabCpy(game, gameSave);

					if (prof % 2 == profStart % 2)
						(*root)->val = max;
					else
						(*root)->val = min;
					n++;
				}
			}
		}
	}

	return (*root)->val;

}

void printNode(list* node) {

	if (!node) {
		printf("emptyList\n");
		return;
	}

	if (node->root) printf("[%d]->", node->root->val);
	printNode(node->next);
	printTree(node->root);

}


void printTree(tree* root) {

	if (!root) return;
	int n = 0;
	printf("[[%d : (%c ; %d)]] : ", root->val, root->strike[1] + 'A', root->strike[0] + 1);
	while (root->child[n]) {
		printf("[%d : (%c ; %d)] -> ", root->child[n]->val, root->child[n]->strike[1] + 'A', root->child[n]->strike[0] + 1);
		n++;
	}
	printf("\n");
	n = 0;
	while (root->child[n]) {
		printTree(root->child[n]);
		n++;
	}

}

int CaseJouable(int game[8][8])
{
	int gameSave[8][8];
	int a=0;
	for (int i = 0; i < 8; i++) {
		for (int j = 0; j < 8; j++) {
			if (verify(game, i, j, 1, 0)) {
				gameSave[i][j] = -2;
				a++;
			}
				
		}
	}
	if (a == 0)
		return 0;
	else
		return gameSave;
}

int countCurrent(int game[8][8])
{
	int scoreJ = 0;
	for (int i = 0; i < 8; i++) {
		for (int j = 0; j < 8; j++) {
			if (game[i][j] == 1)
				scoreJ++;
		}
	}
	return scoreJ;
}

int countRound(int game[8][8])
{
	int scoreR = 0;
	for (int i = 0; i < 8; i++) {
		for (int j = 0; j < 8; j++) {
			if (game[i][j] == -1 || game[i][j]==1)
				scoreR++;
		}
	}
	return scoreR;
}

void initGame(int game[8][8]) {

	for (int i = 0; i < 8; i++) {
		for (int j = 0; j < 8; j++) {
			game[i][j] = 0;
		}
	}
	game[3][3] = 1;
	game[4][4] = 1;
	game[3][4] = -1;
	game[4][3] = -1;

}

void tabCpy(int tab1[8][8], int tab2[8][8]) {

	for (int i = 0; i < 8; i++) {
		for (int j = 0; j < 8; j++) {
			tab1[i][j] = tab2[i][j];
		}
	}

}