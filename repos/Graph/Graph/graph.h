#pragma once

typedef struct sGraph {

	char m_data;
	struct sList* m_child;

}graph;

typedef struct sList {

	char m_data;
	graph* m_node;
	struct sList* m_next;

}list;

list* createList(int p_data);

graph* createGraph(int p_data);

void addNode(graph** node, int p_data);

void freeList(list** head);

void freeGraph(graph** node);