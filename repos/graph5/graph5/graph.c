#include "graph.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

graph* createGraph(int const nb_node) {

	graph* g = (graph*)calloc(1, sizeof(graph));

	g->nbNoeuds = nb_node;
	g->noeuds = (Node*)calloc(nb_node, sizeof(Node));

	for (int i = 0; i < nb_node; i++) g->noeuds[i].id = i;

	return g;

}

void printGraph(graph* g) {

	for (int i = 0; i < g->nbNoeuds; i++) {
		for (int j = 0; j < g->nbNoeuds; j++) {

			printf("|%d|",g->temps[i][j]);

		}
		printf("\n");
	}

}

/*
void addArc(Node* n1, Node* n2, int t) {

	if (!(n1->voisins)) n1->voisins = (Arc*)calloc(1, sizeof(Arc));
	if (!(n2->voisins)) n2->voisins = (Arc*)calloc(1, sizeof(Arc));

	Arc* tmp = (Arc*)calloc(1, sizeof(Arc));

	tmp->cible = n2;
	tmp->pond = t;
	tmp->next = n1->voisins;
	n1->voisins = tmp;

	tmp->cible = n1;
	tmp->next = n2->voisins;
	n2->voisins = tmp;

}*/

void dijkstra(graph* g, int index) {

	int f[6];
	int a[6];
	int t[6] = { 0,0,0,0,0,0 };
	t[index] = 0;

	for (int i = 0; i < g->nbNoeuds; i++) f[i] = g->temps[i][index];

}