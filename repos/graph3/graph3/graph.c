#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "graph.h"

void printGraph(Node* node) {

	int i = 0;
	Node* tmp = (Node*)calloc(1, sizeof(Node));
	tmp = node;
	while (i != 81) {

		if (i % 9 == 0 && i != 0)printf("\n");
		printf("%d	", tmp->color);
		i++;
		tmp = tmp->m_next;

	}printf("\n");

}

Node* createNode(int color) {

	Node* node = (Node*)calloc(1, sizeof(Node));
	node->color = color;
	node->m_neighbors = (Edge*)calloc(1, sizeof(Edge));
	node->m_next = NULL;
	return node;

}

void addNode(Graph** graph, Node** node, int color) {

	if (!node) return;

	if (!*node) {
		*node = createNode(color);
		(*graph)->m_end = *node;
		return;
	}

	addNode(graph, &((*node)->m_next),color);

}

Node* find(Graph* p_graph, int color) {

	if (!p_graph) return NULL;

	Node* tmp = p_graph->m_begin;

	while (tmp) {

		if (tmp->color == color) return tmp;

		tmp = tmp->m_next;

	}

	return NULL;

}

void addNeighbour(Node* p_src, Node* p_dest) {

	Edge* tmp = p_src->m_neighbors;

	while (tmp->m_node) {

		if (tmp->m_node->color == p_dest->color) {

			tmp->m_proba = 1;
			p_src->m_neighbors = tmp;
			return;

		}

		tmp = tmp->m_next;

	}printf("Ajout : %s\n", p_dest->color);

	Edge* arc = (Edge*)calloc(1, sizeof(Edge));
	arc->m_node = p_dest;
	arc->m_next = p_src->m_neighbors;
	arc->m_proba = 1;
	p_src->m_neighbors = arc;

}

void destroy(Graph* graph) {

	Node* tmp = graph->m_begin;

	while (tmp) {

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

int col(Node* node[9][9]) {

	for (int i = 0; i < 9;i++) {
		for (int j = 0; j < 9; j++) {
			
			if (node[j][i]->color == 0) return 1;

		}
	}return 0;

}

void WP(Node* node[9][9]) {

	Edge* n = NULL;
	int k;
	int min;
	int colors[9] = { 1,2,3,4,5,6,7,8,9};

	while (col(node)) {

		for (int i = 0; i < 9;i++) {
			for (int j = 0; j < 9; j++) {
				k = 1;
				min = 81;
				if (node[j][i]->color == 0) {
					n = node[j][i]->m_neighbors;
					while (n) {

						if (n->m_node->color < min && n->m_node->color > 1) min = n->m_node->color;
						printf("Pour tab[%d][%d] = %d, je regarde %d et min = %d\n", j, i, node[j][i]->color, n->m_node->color,min);

						n = n->m_next;

					}node[j][i]->color = min - 1;
				}

			}
			
		}

	}

}