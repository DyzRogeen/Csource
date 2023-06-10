#include "AVL.h"
#include <stdio.h>
#include <stdlib.h>

avl* createNode(int p_data) {
	avl* node = (avl*)calloc(1, sizeof(avl));
	node->m_data = p_data;
	node->tab[GAUCHE] = NULL;
	node->tab[DROIT] = NULL;
	return node;
}

avl** findNode(avl** tree, int p_data) {
	if (!tree) return NULL;
	if (!*tree) return tree;
	if ((*tree)->m_data = p_data) return tree;
	if ((*tree)->m_data < p_data) return findNode(&((*tree)->tab[DROIT]), p_data);
	if ((*tree)->m_data > p_data) return findNode(&((*tree)->tab[GAUCHE]), p_data);
}

avl** insertNode(avl** tree, int p_data) {

	avl** tmp;

	tmp = findNode(tree, p_data);

	if (!*tmp) *tmp = createNode(p_data);

}

void rotR(avl** tree) {

	if (!tree) return;
	if (!*tree || !(*tree)->tab[GAUCHE]) return;
	avl* tmp = (*tree)->tab[GAUCHE];
	if (tmp->tab[DROIT]) (*tree)->tab[GAUCHE] = tmp->tab[DROIT];
	tmp->tab[DROIT] = *tree;
	*tree = tmp;

}

void rotL(avl** tree) {

	if (!tree) return;
	if (!*tree || !(*tree)->tab[DROIT]) return;
	avl* tmp = (*tree)->tab[DROIT];
	if (tmp->tab[GAUCHE]) (*tree)->tab[DROIT] = tmp->tab[GAUCHE];
	tmp->tab[GAUCHE] = *tree;
	*tree = tmp;

}

int F(avl* tree) {

	if (!tree) return 0;
	if (!tree) return 1;
	int fd = 0, fg = 0;
	fd = F(tree->tab[DROIT]);
	fg = F(tree->tab[GAUCHE]);
	return fd - fg;

}