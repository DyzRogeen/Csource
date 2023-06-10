#include "fonc.h"
#include <stdlib.h>
#include <stdio.h>

tree * createTree(int val) {

	tree * root = (tree*)calloc(1, sizeof(tree));
	root->val = val;
	return root;

}

void printGame(int game[8][8]) {

	for (int i = 0; i < 8; i++) {

		for (int j = 0; j < 8; j++) {


			if (game[i][j] == 0) printf("   ");
			if (game[i][j] == 1) printf(" O ");
			if (game[i][j] == -1) printf(" @ ");
			printf("|");

		}

		printf("|\n");
		if (i!=7) printf("---+---+---+---+---+---+---+---||\n");

	}

	printf("================================\n");

}