#pragma once


typedef struct Stree {

	struct Slist* data[5];
	int val;

}tree;

typedef struct Slist {

	tree * root;
	struct Slist* next;

}list;

tree * createTree(int val);

void printGame(int game[8][8]);