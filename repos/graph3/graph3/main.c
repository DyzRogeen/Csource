#define _CRT_SECURE_NO_WARNINGS
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "graph.h"

int main(int argv, char** argc) {

	FILE* f = fopen("sudoku_00.txt", "r");

	if (!f) {
		printf("Couldn't open file.\n");
		return 0;
	}

	int t[9][9];
	Node* tab[9][9];
	int c;
	int u;

	for (int i = 0; i < 9; i++) {
		for (int j = 0; j < 9; j++) {
			if ((c = fgetc(f)) > '9' || c < '0') {
				j--;
				continue;
			}
			t[j][i] = c - '0';
		}
	}

	for (int i = 0; i < 9; i++) {
		for (int j = 0; j < 9; j++) {
			
			tab[j][i] = createNode(t[j][i]);
			if (j == 0 && i!=0) tab[8][i - 1]->m_next = tab[j][i];
			else if (j!=0)
				tab[j-1][i]->m_next = tab[j][i];
		}
	}

	for (int i = 0; i < 9; i++) {
		for (int j = 0; j < 9; j++) {

			Edge* n[20];
			u = 0;

			for (int k = 0; k < 9; k++) {

				if (k != i) {
					//printf("Added tab[%d][%d] = %d to tab[%d][%d] = %d\n",j,k,tab[j][k]->color,j,i, tab[j][i]->color);
					n[u] = (Edge*)calloc(1, sizeof(Edge));
					n[u]->m_node = tab[j][k];
					if (u != 0) n[u - 1]->m_next = n[u];
					u++;

				}

				if (k != j) {
					//printf("Added tab[%d][%d] = %d to tab[%d][%d] = %d\n", k, i, tab[k][i]->color, j, i, tab[j][i]->color);
					n[u] = (Edge*)calloc(1, sizeof(Edge));
					n[u]->m_node = tab[k][i];
					if (u != 0) n[u - 1]->m_next = n[u];
					u++;

				}

			}

			for (int k = i - i % 3; k < i - i % 3 + 3; k++) {
				for (int l = j - j % 3; l < j - j % 3 + 3; l++) {
					if (k != i && l != j) {
						//printf("Added tab[%d][%d] = %d to tab[%d][%d] = %d\n", k, l, tab[k][l]->color, j, i, tab[j][i]->color);
						n[u] = (Edge*)calloc(1, sizeof(Edge));
						n[u]->m_node = tab[k][l];
						n[u - 1]->m_next = n[u];
						u++;
					}
				}
			}

			tab[j][i]->m_neighbors = n;

		}
	}

	printf("///\n");

	printGraph(tab[0][0]);

	WP(tab);
	printf("\n///\n");
	printGraph(tab[0][0]);
}