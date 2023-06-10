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
	int ligne;
	int colone;
	unsigned char couleur;
	Arc* voisins[4];
	int nbVoisins;

};

struct sArc {

	Node* cible;
	int pond;

};

graph* createGraph();

void addArc(Node* n1, Node* n2, int t);

Node* findNode(graph* g, char* s);

void printMat(graph* g);

void floyd(graph* g);

void dijkstra(graph* g, int index);

void printGraph(graph* g);