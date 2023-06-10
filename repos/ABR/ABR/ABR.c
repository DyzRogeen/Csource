#include <stdlib.h>
#include <stdio.h>
#include "ABR.h"

void affTree(abr* tree) {
	if (!tree) { printf("NULL\n"); return; }
	printf("   [%d]\n", tree->m_data);
	if (tree->tab[GAUCHE] || tree->tab[DROIT]) {

		printf("  /   \\ \n");
		if (tree->tab[GAUCHE]) { printf("[%d] ", tree->tab[GAUCHE]->m_data); }
		else { printf("NULL"); }
		if (tree->tab[DROIT]) { printf("  [%d]", tree->tab[DROIT]->m_data); }
		else { printf(" NULL"); }

	}
	printf("\n\n");
	if (tree->tab[GAUCHE]) affTree(tree->tab[GAUCHE]);
	if (tree->tab[DROIT]) affTree(tree->tab[DROIT]);
	
}

abr* createNode(int p_data) {
	abr* node = (abr*)calloc(1, sizeof(abr));
	node->m_data = p_data;
	node->tab[GAUCHE] = NULL;
	node->tab[DROIT] = NULL;
	return node;
}

list* createList(int p_data) {

	list* root = (list*)calloc(1, sizeof(list));
	root->m_data = p_data;
	root->m_next = NULL;
	return root;

}

abr** findNode(abr** tree, int p_data) {
	if (!tree) return NULL;
	if (!*tree) return tree;
	if ((*tree)->m_data == p_data) return tree;
	if ((*tree)->m_data < p_data) return findNode(&((*tree)->tab[DROIT]), p_data);
	if ((*tree)->m_data > p_data) return findNode(&((*tree)->tab[GAUCHE]), p_data);
}

void addNode(abr** tree, int p_data) {
	
	abr** tmp = tree;

	tmp = findNode(tree, p_data);

	if (!*tmp) {

		*tmp = createNode(p_data);

		}
	
}

void supprNode(abr** tree, int p_data) {

	abr** tmp;
	abr* tmp2;

	int i = 0, j = 0;

	tmp = findNode(tree, p_data);

	if (!*tmp) return;
	//tmp2 = (*tmp)->tab[GAUCHE]; free((*tmp)->tab[GAUCHE]); *tmp = tmp2; return;
	if (!(*tmp)->tab[DROIT] && !(*tmp)->tab[GAUCHE]) { free(*tmp); *tmp = NULL; return; }
	if (!(*tmp)->tab[DROIT] && (*tmp)->tab[GAUCHE]) { (*tmp)->m_data = (*tmp)->tab[GAUCHE]->m_data; free((*tmp)->tab[GAUCHE]); (*tmp)->tab[GAUCHE] = NULL; return; }
	if ((*tmp)->tab[DROIT] && !(*tmp)->tab[GAUCHE]) { (*tmp)->m_data = (*tmp)->tab[DROIT]->m_data; free((*tmp)->tab[DROIT]); (*tmp)->tab[DROIT] = NULL; return; }
	if ((*tmp)->tab[DROIT] && (*tmp)->tab[GAUCHE]) {
		for (i = 0; findNode(tmp, p_data + i)!=NULL; i++);
		for (j = 0; findNode(tmp, p_data - j)!=NULL; j++);
		printf("SuppNode : i = %d\n", i);
		if (i > j) {
			(*tmp)->m_data = p_data - j;
			supprNode(tree, p_data - j);
			return;
		}
		else {
			(*tmp)->m_data = p_data + i;
			supprNode(tree, p_data + i);
		}
	}

}

list* linear(abr* tree, list* root) {

	if (!tree) return root;
	root = linear(tree->tab[DROIT], root);
	tree->tab[DROIT] = root;
	root = linear(tree->tab[GAUCHE], root);
	tree->tab[GAUCHE] = root;
	return root;

}

abr* arbor(abr* tree, list* root, int leng) {

	for (int i = 0; i < leng / 2; i++) { root = root->m_next; }
	//SI MILIEU TROUVE
	/*
	ddNode(&tree, root->m_data);
	tree->tab[DROIT] = arbor(tree, root->m_next);
	tree->tab[GAUCHE] = arbor(tree, root);
	return tree;
	*/
}

void push(list** root, int p_data) {

	if (!root) return;
	list* tmp = createList(p_data);
	tmp->m_next = *root;
	*root = tmp;

}

int F(abr* tree) {

	if (!tree) return -1;
	if (!tree->tab[DROIT] && !tree->tab[GAUCHE]) return 0;
	int fd = -1, fg = -1;
	if (tree->tab[DROIT]) fd = F(tree->tab[DROIT]);
	if (tree->tab[GAUCHE]) fg = F(tree->tab[GAUCHE]);
	return fd - fg;

}