#include "Pixel.h"
#include <stdlib.h>
#include <stdio.h>

treeNode* creerNoeudArbre(int p_value) {

	treeNode* tree = (treeNode*)calloc(1, sizeof(treeNode));
	tree->m_data = p_value;
	return tree;

}

void addNode(treeNode** p_pRoot, int index) {

	if (!*p_pRoot || !(*p_pRoot)->tab[index]) {

		(*p_pRoot)->tab[index] = creerNoeudArbre(0);
		return;

	}

}