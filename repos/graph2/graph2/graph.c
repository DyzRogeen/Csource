#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "graph.h"

void printGraph(Node* begin, Node* end, Graph* graph) {

	if (begin->m_letters=='1') return;

	printf("%s->", begin->m_letters);

	Edge* tmp = begin->m_neighbors;

	while (tmp) {
		printGraph(tmp->m_node, end, graph);
		tmp = tmp->m_next;
	}printf("\n");

}

Node* createNode(unsigned char p_letters) {

	Node* node = (Node*)calloc(1, sizeof(Node));
	node->m_letters = (char*)calloc(2, sizeof(char));
	node->m_letters[0] = p_letters;
	node->m_neighbors = (Edge*)calloc(1, sizeof(Edge));
	node->m_next = NULL;
	return node;

}

Graph* createGraph(unsigned char* p_letters) {

	Graph* graph = (Graph*)calloc(1, sizeof(Graph));
	graph->m_begin = createNode(p_letters);
	graph->m_end = graph->m_begin;
	return graph;

}

void addNode(Graph** graph, Node** node, unsigned char* p_letters) {

	if (!node) return;

	if (!*node) {
		*node = createNode(p_letters);
		(*graph)->m_end = *node;
		return;
	}
	
	addNode(graph, &((*node)->m_next), p_letters);

}

Node* find(Graph* p_graph, unsigned char* p_letters) {

	if (!p_graph) return NULL;

	Node* tmp = p_graph->m_begin;

	while (tmp) {

		if (!strcmp(tmp->m_letters, p_letters)) return tmp;

		tmp = tmp->m_next;

	}

	return NULL;

}

void addNeighbour(Node* p_src, Node* p_dest) {

	Edge* tmp = p_src->m_neighbors;

	while (tmp->m_node) {

		if (!strcmp(tmp->m_node->m_letters,p_dest->m_letters)) {

			printf("Voisin trouve : %s et %s\n", tmp->m_node->m_letters, p_dest->m_letters);
			tmp->m_proba = 1;
			p_src->m_neighbors = tmp;
			return;

		}

		tmp = tmp->m_next;

	}printf("Ajout : %s\n", p_dest->m_letters);

	Edge* arc = (Edge*)calloc(1, sizeof(Edge));
	arc->m_node = p_dest;
	arc->m_next = p_src->m_neighbors;
	arc->m_proba = 1;
	p_src->m_neighbors = arc;

}

void destroy(Graph* graph) {

	Node* tmp = graph->m_begin;

	while (tmp) {

		if (tmp->m_letters) free(tmp->m_letters);
		
		while (tmp->m_neighbors) {
			Edge* l = tmp->m_neighbors;
			tmp->m_neighbors = tmp->m_neighbors->m_next;
			free(l);

		}
		
		Node* tmp2 = tmp;
		tmp = tmp->m_next;
		free(tmp2);

	}
	
}