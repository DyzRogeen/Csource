#pragma once

typedef struct AVL {

	int m_data;
	struct AVL* tab[2];

}avl;

enum{DROIT,GAUCHE};

avl* createNode(int p_data);

avl** findNode(avl** tree, int p_data);

avl** insertNode(avl** tree, int p_data);

void rotR(avl** tree);

void rotL(avl** tree);

int F(avl* tree);

