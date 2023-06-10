#pragma once

typedef struct ABR {

	int m_data;
	struct ABR* tab[2];

}abr;

typedef struct LIST {

	int m_data;
	struct LIST* m_next;

}list;

enum{DROIT, GAUCHE};

void affTree(abr* tree);

abr* createNode(int p_data);

abr** findNode(abr** tree, int p_data);

void addNode(abr** tree, int p_data);

void supprNode(abr** tree, int p_data);

list* linear(abr* tree, list* root);

abr* arbor(abr* tree, list* root);

void push(list** root, int m_data);

list* createList(int p_data);

int F(abr* tree);