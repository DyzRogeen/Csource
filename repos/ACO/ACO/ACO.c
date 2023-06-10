#define _CRT_SECURE_NO_WARNINGS 0
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "ACO.h"

double dist(Node* n1, Node* n2) {

	return sqrt(pow(n1->x - n2->x, 2) + pow(n1->y - n2->y, 2));

}

graph* createGraph() {

	FILE* f = fopen("wi29.tsp", "r");
	int nbNode;
	double x, y;

	if (!f) {
		printf("Erreur\n");
		return 0;
	}

	fscanf_s(f,"%d\n", &nbNode);

	double tab[29][29];

	graph* g = (graph*)calloc(1, sizeof(graph));
	Node** node = (Node**)calloc(nbNode, sizeof(Node*));

	g->nbNode = nbNode;

	for (int i = 0; i < nbNode; i++) {
		node[i] = (Node*)calloc(1, sizeof(Node));
		fscanf_s(f, "%d %lf %lf\n", &(node[i]->id), &(node[i]->x), &(node[i]->y));
	}

	g->node = node;

}

void ACO(graph* g, Node* n1, Node* n2) {

	Node** node = g->node;

	double tab[29][29];
	float P[29][29];
	int c[29];
	int L;
	int id;

	for (int i = 0; i < g->nbNode; i++) {
		for (int j = i; j < g->nbNode; j++) {
			tab[i][j] = dist(node[i], node[j]);
			P[i][j] = 0.0;
		}
	}

	for (int i = 0; i < 10; i++) {

		id = n1->id;

		for (int j = 0; i < g->nbNode; j++) c[j] = 0;

		while (id != n2->id) {
			L = 0;

			//L += tab[][];
		}

		for (int j = 0; i < g->nbNode; j++) {
			for (int k = j; k < g->nbNode; k++) {
				//P += 1/L
			}
		}

	}

}