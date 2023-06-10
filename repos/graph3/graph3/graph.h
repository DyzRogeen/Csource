#pragma once

typedef struct sGraph Graph;
typedef struct sNode Node;
typedef struct sEdge Edge;

struct sGraph
{
	Node* m_begin;
	Node* m_end;
};

struct sNode
{
	int color;
	Edge* m_neighbors;
	Node* m_next;
};

struct sEdge
{
	double m_proba;
	Node* m_node;
	Edge* m_next;
};

void printGraph(Node* node);

Node* createNode(int color);

Graph* createGraph(int color);

void addNode(Graph** graph, Node** node, int color);

Node* find(Graph* p_graph, int color);

void addNeighbour(Node* p_src, Node* p_dest);

void WP(Node* node[9][9]);