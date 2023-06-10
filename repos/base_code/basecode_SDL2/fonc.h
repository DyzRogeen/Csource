#pragma once


typedef struct Stree {

	int val;
	int strike[2];
	struct Stree* child[60];

}tree;

typedef struct Slist {

	tree* root;
	struct Slist* next;

}list;

tree* createTree(int val);

//list* createList(int val);

//void addNode(list** head, int val);

void printGame(int game[8][8]);

void turnJoueur(int game[8][8],int lin, int col,  int turn);

void turnIA(int game[8][8], tree* root);

int verify(int game[8][8], int col, int lin, int turn, int modify);

int victory(int game[8][8]);

int countCurrent(int game[8][8]);

int countRound(int game[8][8]);

int initTreeIA(int game[8][8], tree** root, int prof, int turn);

void printNode(list* node);
void printTree(tree* root);

void initGame(int game[8][8]);

void tabCpy(int tab1[8][8], int tab2[8][8]);