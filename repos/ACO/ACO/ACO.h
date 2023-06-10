#pragma once

typedef struct sNode {

	int id;
	int x;
	int y;

}Node;

typedef struct sGraph {

	int nbNode;
	Node** node;
	double *tab[29][29];

}graph;

double dist(Node* n1, Node* n2);

graph* createGraph();

