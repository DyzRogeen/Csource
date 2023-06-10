#include "fonc.h"
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

tree * createTree(int val) {

	tree * root = (tree*)calloc(1, sizeof(tree));
	root->val = val;
	return root;

}

list* createList(int val) {

	list* head = (list*)calloc(1, sizeof(list));
	head->root = createTree(val);
	return head;

}

void addNode(list** head, int val) {

	if (!*head) {

		head = createList(val);
		return;

	}

	list* tmp = *head;
	while ((*head)->next) *head = (*head)->next;
	(*head)->next = createList(val);
	*head = tmp;

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

		printf("| |%d\n",i + 1);
		if (i!=7) printf("||---+---+---+---+---+---+---+---|| |\n");

	}

	printf("[]===============================[] |\n");
	printf("\\__________________________________\\|\n");
	printf("   A   B   C   D   E   F   G   H\n");

}

void turnJoueur(int game[8][8], int turn) {

	int col = 0, lin = 0;
	char c = '\0';
	char coup[3];

	bool choix_De_Coup = true;

	//Demande de Placement
	while (choix_De_Coup){

		printf("Que jouer ? (format : colonne(A-H) ligne(1-8)) : ");
		scanf_s("%s", coup, 3);
		col = coup[0] - 'A';
		lin = coup[1] - '1';

		if (0 > col || 0 > lin || 7 < col || 7 < lin || (game[lin][col]) != 0) {

			printf("ERREUR : Coup non valide !\n");

		}

		else if (verify(game, lin, col, turn, 1)) choix_De_Coup = false;
		else printf("ERREUR : Coup non efficace !\n");
		
	}

}

void turnIA(int game[8][8]) {
	//TODO
}

int verify(int game[8][8], int col, int lin, int turn, int modify) {
	
	int k = 0, l = 0, a = 1;
	bool Ally_Found = false;
	bool coup_Eff = false;
	
	for (int i = -1; i <= 1; i++) {

		for (int j = -1; j <= 1; j++) {

			if (i == 0 && j == 0) continue;
			
			if (game[col + i][lin + j] == turn * -1) {

				k = i;
				l = j;
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

	int res = countCurrent(game);

	if (prof == 0) {

		addNode((*root)->val, res);
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

			if (game[i][j] == 0)
				if (verify(game, i, j, turn, 1)) {

					initTreeIA(game, root, prof - 1, turn * -1);

					for (int i = 0; i < 8; i++) {
						for (int j = 0; j < 8; j++) {

							game[i][j] = gameSave[i][j];

						}
					}

				}

		}
	}

}