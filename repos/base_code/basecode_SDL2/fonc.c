#include "fonc.h"
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

tree* createTree(int val) {

	tree* root = (tree*)calloc(1, sizeof(tree));
	root->val = val;
	return root;

}

/*
void libererArbre(tree** root) {

	if (!(*p_pRoot))return;
	if ((*p_pRoot)->tab[GAUCHE] == NULL && (*p_pRoot)->tab[DROITE] == NULL) {
		free(*p_pRoot);
		return;
	}

	libererArbre(&((*p_pRoot)->tab[GAUCHE]));
	libererArbre(&((*p_pRoot)->tab[DROITE]));
	return;

}*/
/*
list* createList(int val) {

	list* head = (list*)calloc(1, sizeof(list));
	head->root = createTree(val);
	return head;

}


void addNode(list** head, int val) {

	if (!head) return;
	
	if (!*head) {

		*head = createList(val);
		printf("ADDED\n");
		return;

	}

	addNode(&((*head)->next), val);

}
*/
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


		if (0 > col || 0 > lin || 7 < col || 7 < lin || (game[lin][col]) != 0) {

			printf("ERREUR : Coup non valide !\n");
			return;
		}

		else if (verify(game, lin, col, turn, 1)) {
			choix_De_Coup = false;
			return;
		}
		else {
			printf("ERREUR : Coup non efficace !\n");
			return;
		}

	}

}

void turnIA(int game[8][8], tree* root) {

	int i = root->strike[0];
	int j = root->strike[1];
	

	verify(game, i, j, -1, 1);
	printf("Le coup joué est %d %d \n", i, j);
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

	int scoreJ = 0, scoreIA = 0;

	//Etabli le score
	for (int i = 0; i < 8; i++) {
		for (int j = 0; j < 8; j++) {

			if (game[i][j] == 1)
				scoreJ++;
			if (game[i][j] == -1)
				scoreIA++;

		}
	}

	//Retourne le score du gagant
	if (scoreJ > scoreIA)
		return scoreJ;
	return scoreIA * -1;

}

int countCurrent(int game[8][8]) {

	int score = 0;

	for (int i = 0; i < 8; i++) {
		for (int j = 0; j < 8; j++) {

			if (game[i][j] == -1)
				score++;

		}
	}

	return score;

}

int countRound(int game[8][8]) {

	int score = 0;

	for (int i = 0; i < 8; i++) {
		for (int j = 0; j < 8; j++) {

			if (game[i][j] != 0)
				score++;

		}
	}
	return score;
}

int initTreeIA(int game[8][8], tree** root, int prof, int turn) {

	int res = -1;
	int min = 64;
	int max = 0;
	int n = 0;
	//list** rootSave = &((*root)->child);

	if (prof == 0) {

		res = countCurrent(game);
		(*root)->val = res;
		return res;

	}

	//Sauvegarde du plateau propre à la racine
	int gameSave[8][8];

	for (int i = 0; i < 8; i++) {
		for (int j = 0; j < 8; j++) {

			gameSave[i][j] = game[i][j];

		}
	}

	//Enumère les possibilités du jeu actuel
	for (int i = 0; i < 8; i++) {
		for (int j = 0; j < 8; j++) {

			//printf("Je verifie (%d, %d)...\n", i, j);

			if (game[i][j] == 0)
				
				if (verify(game, i, j, turn, 1)) {

					//printf("Prof : %d ; (%d, %d) est une case jouable\n", prof, i, j);

					(*root)->child[n] = (tree*)calloc(1, sizeof(tree));

					(*root)->child[n]->strike[0] = i;
					(*root)->child[n]->strike[1] = j;

					res = initTreeIA(game, (*root)->child + n, prof - 1, turn * -1);

					(*root)->child[n]->val = res;

					//printf("Prof : %d ; %d retournee\n", prof, res);

					if (prof % 2 == 0) {

						if (res > max) {

							max = res;
							//printf("Max trouve a (%d, %d) : %d\n", i, j, max);
							if (prof == 2) {

								(*root)->strike[0] = i;
								(*root)->strike[1] = j;

							}
							
						}

					}

					else {

						if (res < min) {

							min = res;
							//printf("Min trouve a (%d, %d) : %d\n", i, j, min);

						}

					}

					for (int i = 0; i < 8; i++) {
						for (int j = 0; j < 8; j++) {

							game[i][j] = gameSave[i][j];

						}
					}

					if (prof % 2 == 0)
						(*root)->val = max;
					else
						(*root)->val = min;

					n++;
					
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

	printf("[[%d : (%d ; %d)]] : ", root->val, root->strike[0], root->strike[1]);

	while (root->child[n]) {

		printf("[%d : (%d ; %d)] -> ", root->child[n]->val, root->child[n]->strike[0], root->child[n]->strike[1]);
		n++;

	}
	printf("\n");
	n = 0;

	while (root->child[n]) {

		printTree(root->child[n]);
		n++;

	}
	

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