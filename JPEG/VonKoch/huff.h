#pragma once

typedef struct sTree {

	int val;
	char c;
	struct sTree* children[2];

}tree;

tree* createTree(int val, char c);

char seekChar(tree* root, char* path);

void printTree(tree* root);

void freeTree(tree* root);

int charCount(int* t);

int getCouples(int* t, int* tchar, int charCnt);

void sort(int* t, int nbElem);

tree* mkTree(int* t, int nbElem);

char* lookForChar(char c, tree* t, int depth, char* num);

void strreverse(char* s);

void getCode(char** s, tree* t, int nbChar, int* tchar);

void encodeHuff(char** coded, int nbChar, int* tchar);

void decodeHuff();

void huffComp();