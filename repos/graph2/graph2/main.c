#include <stdlib.h>
#include <stdio.h>
#include "graph.h"

int main(int argv, char** argc) {

	//Création du graph avec toutes les lettres de l'alphabet
	Graph* graph = createGraph('0');

	for (char i = 'A'; i <= 'Z'; i++) {

		addNode(&graph, &(graph->m_begin), i);

	}

	addNode(&graph, &(graph->m_begin), '1');
	
	//Création des Arcs
	Node* src = graph->m_begin;
	Node* dest = NULL;
	char c[2];
	c[1] = '\0';

	while (src != graph->m_end) {

		printf("current node : %s\n", src->m_letters);

		c[0] = '1';
		dest = find(graph, c);
		addNeighbour(src, dest);

		for (char i = 'Z'; i >= 'A'; i--) {

			c[0] = i;
			dest = find(graph, c);
			addNeighbour(src, dest);

		}

		src = src->m_next;

	}

	printGraph(graph->m_begin, graph->m_end, graph);

	destroy(graph);

}