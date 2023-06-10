#include "utils.h"
#include <stdio.h>
#include <stdlib.h>

int main(int argv, char** argc) {

	int plateau[8][8];
	int niveau[8];
	int turn = 1;

	for (int i = 0; i < 8; i++) {
		niveau[i] = 0;
		for (int j = 0; j < 8; j++)
			plateau[i][j] = 0;
	}
	printGame(plateau);
	while (tourJoueur(plateau, niveau, turn) == 0) turn *= -1;

	printf("Fin du jeu !\n");

}