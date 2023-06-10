#pragma once

typedef struct sGraph graph;

typedef struct sNode Node;

typedef struct sArc Arc;

struct sGraph {

	Node* noeuds;
	int nbNoeuds;
	int** temps;

};

struct sNode {

	int id;
	char label[128];
	int categorie;
	Arc* voisins;

};

struct sArc {

	Node* cible;
	int temps;
	struct sArc* next;

};

graph* createGraph();

void ajouteArc(Node* n1, Node* n2, int t);

Node* findNode(graph* g, char* s);

void printMat(graph* g);

void floyd(graph* g);