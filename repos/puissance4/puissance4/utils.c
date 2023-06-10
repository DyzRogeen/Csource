#define _CRT_SECURE_NO_WARNINGS false

#include "utils.h"
#include <stdio.h>
#include <stdlib.h>

int tourJoueur(int plateau[8][8], int niveau[8], int turn) {

	int move;

	do {
		scanf("%d", &move);
	} while (niveau[move] == 8);

	plateau[--move][niveau[move]++] = turn;

	printGame(plateau);

	return verifGame(plateau, niveau, move, turn);

}

void printGame(int plateau[8][8]) {

	printf("[]===============================[]\\\n");

	for (int i = 0; i < 8; i++) {

		printf("||");

		for (int j = 0; j < 8; j++) {

			if (plateau[j][7 - i] == 0) printf("   ");
			if (plateau[j][7 - i] == 1) printf(" O ");
			if (plateau[j][7 - i] == -1) printf(" @ ");
			printf("|");

		}

		printf("| |%c\n", i + 'A');
		if (i != 7) printf("||---+---+---+---+---+---+---+---|| |\n");

	}

	printf("[]===============================[] |\n");
	printf("\\__________________________________\\|\n");
	printf("   1   2   3   4   5   6   7   8\n");

}

int verifGame(int plateau[8][8], int niveau[8], int move, int turn) {

	int count;
	for (int r = 0; r < 8; r++) printf("%d ", niveau[r]);
	printf("\n");
	for (int i = -1; i <= 1; i++) {
		
		count = 0;
		for (int k = -3; k <= 3; k++) {

			if (k + move < 0 || k + move > 7 || k * i + niveau[move] - 1 < 0 || k * i + niveau[move] - 1 > 7 || k == 0) continue;


			if (plateau[k + move][k * i + niveau[move] - 1] == turn) count++;
			else count = 0;

			if (count == 3) return turn;

		}
	}
	if (niveau[move] < 4) return 0;

	for (int k = 1; k < 4; k++) 
		if (plateau[move][niveau[move] - 1 - k] != turn)
			return 0;

	return turn;

}