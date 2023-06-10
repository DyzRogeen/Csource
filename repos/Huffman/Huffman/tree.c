#include <stdlib.h>
#include <stdio.h>
#include "tree.h"

tree* createTree(int p_data, char c) {

	tree* root = (tree*)calloc(1, sizeof(tree));
	root->child[0] = NULL;
	root->child[1] = NULL;
	root->m_data = p_data;
	root->m_char = c;
	return root;

}

list* createList(int p_data, char c) {

	list* head = (list*)calloc(1, sizeof(list));
	head->m_next = NULL;
	head->root = createTree(p_data, c);
	return head;

}

void addTree(tree** p_root, int p_data, char c) {

	if (!p_root) return;

	if (!*p_root) {

		tree* new = createTree(p_data, c);
		*p_root = new;
		return;

	}

	addTree(&(*p_root)->child[0], p_data, c);
	addTree(&(*p_root)->child[1], p_data, c);

}

void addList(list** p_head, int p_data, char c) {

	if (!p_head) return;

	if (!*p_head) {

		list* new = createList(p_data, c);
		*p_head = new;
		return;

	}

	addTree(&(*p_head)->m_next, p_data, c);

}

list* listToTree(list** p_head) {

	if (!p_head || !*p_head) return;

	list* new = createList(0, 0);

	new->root->child[0] = createTree((*p_head)->root->m_data, (*p_head)->root->m_char);
	new->root->m_data += (*p_head)->root->m_data;
	if ((*p_head)->m_next) {
		new->root->child[1] = createTree((*p_head)->m_next->root->m_data, (*p_head)->m_next->root->m_char);
		new->root->m_data += (*p_head)->m_next->root->m_data;
		(*p_head)->m_next = listToTree(&(*p_head)->m_next->m_next);
	}
	else {
		new->root->child[1] = createTree(0, 0);
		*p_head = listToTree(&(*p_head)->m_next);
	}

	return new;

	while ((*p_head)->m_next);
	
}