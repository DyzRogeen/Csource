#include <stdlib.h>
#include <stdio.h>
#include "graph.h"

graph* createGraph(int p_data) {

	graph* node = (graph*)calloc(1, sizeof(graph));
	list* head = (list*)calloc(1, sizeof(list));
	head->m_data = p_data;
	head->m_next = NULL;
	head->m_node = NULL;
	node->m_child = head;

}

void addNode(graph** node, int p_data) {

	if (!node) return;
	
	list* tmp = (*node)->m_child;

	if (!tmp) {
		(*node)->m_child->m_node = createGraph(p_data);
		return;
	}

	addNode(&(tmp->m_next), p_data);

}

void freeList(list** head);

void freeGraph(graph** node);