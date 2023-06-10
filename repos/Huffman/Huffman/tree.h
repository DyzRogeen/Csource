#pragma once

typedef struct Slist {

	struct Slist* m_next;
	struct Stree* root;

}list;

typedef struct Stree {

	int m_data;
	char m_char;
	struct Stree* child[2];

}tree;

tree* createTree(int p_data, char c);

list* createList(int p_data, char c);

void addTree(tree** p_root, int p_data, char c);

void addList(list** p_head, int p_data, char c);