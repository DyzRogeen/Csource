#define _CRT_SECURE_NO_DEPRECATE
#include <stdlib.h>
#include <stdio.h>
#include "morse.h"
#define _CRT_SECURE_NO_WARNINGS 1

void affTree(abr* tree) {
	if (!tree) { printf("NULL\n"); return; }
	printf("   [%c]\n", tree->m_data);
	if (tree->tab[LEFT] || tree->tab[RIGHT]) {

		printf("  /   \\ \n");
		if (tree->tab[LEFT]) { printf("[%c] ", tree->tab[LEFT]->m_data); }
		else { printf("NULL"); }
		if (tree->tab[RIGHT]) { printf("  [%c]", tree->tab[RIGHT]->m_data); }
		else { printf(" NULL"); }

	}
	printf("\n\n");
	if (tree->tab[LEFT]) affTree(tree->tab[LEFT]);
	if (tree->tab[RIGHT]) affTree(tree->tab[RIGHT]);

}

void printFile(const char* argv) {

	FILE* file = fopen(argv, "r");
	char car = fgetc(file);
	if (!file) return;
	while (car != EOF) {

		printf("%c", car);
		car = fgetc(file);

	}
	fclose(file);
}

abr* createNode(char car) {
	abr* node = (abr*)calloc(1, sizeof(abr));
	node->m_data = car;
	node->tab[LEFT] = NULL;
	node->tab[RIGHT] = NULL;
	return node;
}

abr** findNode(abr** tree, char* morse) {
	if (!tree) return NULL;
	if (!*tree) *tree = createNode(' ');;
	if (*morse == '\0') return tree;
	if (*morse == '.') {
		if (!(*tree)->tab[LEFT]) (*tree)->tab[LEFT] = createNode(' ');
		return findNode(&((*tree)->tab[LEFT]), morse + 1);
	}
	if (*morse == '-') {
		if (!(*tree)->tab[RIGHT]) (*tree)->tab[RIGHT] = createNode(' ');
		return findNode(&((*tree)->tab[RIGHT]), morse + 1);
	}
}

void addNode(abr** tree, char* morse, char car) {

	if (!tree) return;
	//if (!*tree) *tree = createNode('\0');
	abr** tmp = findNode(tree, morse);
	(*tmp)->m_data = car;

}

char findChar(abr* tree, char* morse) {

	if (!morse) return '\0';

	abr** tmp = findNode(tree, morse);

	return (*tmp)->m_data;

	//if (*morse == '\0') return tree->m_data;
	//if (*morse == '.') return findChar(tree->tab[LEFT], morse + 1);
	//if (*morse == '-') return findChar(tree->tab[RIGHT], morse + 1);

}