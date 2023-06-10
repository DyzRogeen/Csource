#pragma once


typedef struct Stree {

	struct Slist* child[5];
	int val;

}tree;

typedef struct Slist {

	tree * root;
	struct Slist* next;

}list;

tree * createTree(int val);

list* createList(int val);

void addNode(list** head, int val);

void printGame(int game[8][8]);

void turnJoueur(int game[8][8], int turn);

void turnIA(int game[8][8]);

int verify(int game[8][8], int col, int lin, int turn, int modify);

int victory(int game[8][8]);

int countCurrent(int game[8][8]);

int countRound(int game[8][8]);

int initTreeIA(int game[8][8], tree** root, int prof, int turn);