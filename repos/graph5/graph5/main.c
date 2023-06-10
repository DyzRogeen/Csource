#include "graph.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int main(int argv, char** argc) {

	graph* g = createGraph(6);
	/*
	addArc(&g->noeuds[0], &g->noeuds[1], 1);
	addArc(&g->noeuds[0], &g->noeuds[3], 6);
	addArc(&g->noeuds[0], &g->noeuds[4], 4);
	addArc(&g->noeuds[0], &g->noeuds[5], 2);

	addArc(&g->noeuds[1], &g->noeuds[2], 6);
	addArc(&g->noeuds[1], &g->noeuds[5], 3);

	addArc(&g->noeuds[2], &g->noeuds[3], 5);
	addArc(&g->noeuds[2], &g->noeuds[4], 1);
	addArc(&g->noeuds[2], &g->noeuds[5], 2);

	addArc(&g->noeuds[3], &g->noeuds[4], 1);

	addArc(&g->noeuds[4], &g->noeuds[5], 3);*/

	int f[6][6] = { {0,1,-1,6,4,2},{1,0,6,-1,-1,3},{-1,6,0,5,1,2},{6,-1,5,0,1,-1},{4,-1,1,1,0,3},{2,3,2,-1,3,0} };
	g->temps = f;

	printGraph(g);

	dijkstra(g, 1);

}