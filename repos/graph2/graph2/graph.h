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

void printGraph(Node* begin, Node* end, Graph* graph);

Node* createNode(unsigned char* p_letters);

Graph* createGraph(unsigned char* p_letters);

void addNode(Graph** graph, Node** node, unsigned char* p_letters);

Node* find(Graph* p_graph, unsigned char* p_letters);

void addNeighbour(Node* p_src, Node* p_dest);