#define _CRT_SECURE_NO_WARNINGS 1
#include "graph.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

graph* createGraph() {

	FILE* f = fopen("./instance.txt", "r");

	if (!f) {
		printf("Le fichier n'a pu être ouvert.\n");
		return NULL;
	}

	graph* g = (graph*)calloc(1, sizeof(graph));

	if (!g) return NULL;

	fscanf(f, "%d\n", &(g->nbNoeuds));

	g->noeuds = (Node*)calloc(g->nbNoeuds, sizeof(Node));

	if (!(g->noeuds)) {
		free(g->noeuds);
		free(g);
		return NULL;
	}

	for (int i = 0; i < g->nbNoeuds; i++) {

		fscanf(f, "%s\n", g->noeuds[i].label);
		g->noeuds[i].id = i;

	}
	
	for (int i = 0; i < g->nbNoeuds - 1; i++) fscanf(f, "%d ", &(g->noeuds[i].categorie));
	fscanf(f, "%d\n", &(g->noeuds[g->nbNoeuds].categorie));

	g->temps = (int**)calloc(g->nbNoeuds, sizeof(int*));

	if (!(g->temps)) {
		free(g->temps);
		free(g->noeuds);
		free(g);
		return NULL;
	}

	for (int i = 0; i < g->nbNoeuds; i++) {
		g->temps[i] = (int*)calloc(g->nbNoeuds, sizeof(int));
		if (!(g->temps[i])) {
			for (int j = 0; j < i; j++) free(g->temps[j]);
			free(g->temps);
			free(g->noeuds);
			free(g);
			return NULL;
		}
		for (int j = 0; j < g->nbNoeuds; j++) g->temps[i][j] = -1;
		g->temps[i][i] = 0;
	}

	int nbArc;
	char lb1[128],lb2[128];
	int h, m;
	Node* n1, *n2;
	fscanf(f, "%d\n", &nbArc);

	for (int i = 0; i < nbArc; i++) {

		fscanf(f, "%s %s %d %d\n", lb1, lb2, &h, &m);
		n1 = findNode(g, lb1);
		n2 = findNode(g, lb2);
		ajouteArc(n1, n2, h * 60 + m);
		g->temps[n1->id][n2->id] = h * 60 + m;
		//printf("Ajout vol de %s -> %s, duree : %d\n", lb1,lb2, h * 60 + m);

	}

	fclose(f);

	return g;
}

void ajouteArc(Node* n1, Node* n2, int t) {

	if (!(n1->voisins)) n1->voisins = (Arc*)calloc(1, sizeof(Arc));
	if (!(n2->voisins)) n2->voisins = (Arc*)calloc(1, sizeof(Arc));

	Arc* a = (Arc*)calloc(1, sizeof(Arc));

	a->cible = n2;
	a->temps = t;
	a->next = n1->voisins;
	n1->voisins = a;

}

Node* findNode(graph* g, char* s) {

	for (int i = 0; i < g->nbNoeuds; i++) {
		if (!strcmp(g->noeuds[i].label, s)) return &(g->noeuds[i]);
	}
	return NULL;
}

void printMat(graph* g) {

	printf("     ");
	for (int i = 0; i < g->nbNoeuds; i++) printf("| %s ", g->noeuds[i].label);
	printf("\n");

	for (int i = 0; i < g->nbNoeuds; i++) {

		printf(" %s ", g->noeuds[i].label);

		for (int j = 0; j < g->nbNoeuds; j++)printf("| %d ", g->temps[i][j]);

		printf("\n");

	}

}

void floyd(graph* g) {

	for (int i = 0; i < g->nbNoeuds; i++) {
		for (int j = 0; j < g->nbNoeuds; j++) {
			for (int k = 0; k < g->nbNoeuds; k++) {
				if (g->temps[i][k] != -1 && g->temps[k][j] != -1 && (g->temps[i][j] > g->temps[i][k] + g->temps[k][j] || g->temps[i][j] == -1)) {
					g->temps[i][j] = g->temps[i][k] + g->temps[k][j];
					printf("%s -> %s = %s -> %s + %s -> %s\n", g->noeuds[i].label, g->noeuds[j].label, g->noeuds[i].label, g->noeuds[k].label, g->noeuds[k].label, g->noeuds[j].label);
					floyd(g);
				}
			}
		}
	}

}