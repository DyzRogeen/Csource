#define _CRT_SECURE_NO_WARNINGS
#include <stdlib.h>
#include <stdio.h>
#include "fileBin.h"
#include "tree.h"

int main(int argv, char** argc) {

	FILE* f = fopen("./Texte.txt", "r");

	if (!f) {

		printf("Le fichier n'a pas pu être ouvert.\n");
		return 0;

	}

	int tab[127], i = 0, c = 0;
	list** head = NULL;
	tree** root = NULL;

	for (i = 0; i < 127; i++)tab[i] = 0;
	for (i = 0;(c = freadByte(f))!=0; i++) tab[c]++;
	for (i = 0; i < 127; i++) {
		if (tab[i] != 0)
			addList(head, tab[i],i);
	}
}