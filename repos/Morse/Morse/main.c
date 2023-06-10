#define _CRT_SECURE_NO_DEPRECATE
#include <stdlib.h>
#include <stdio.h>
#define _CRT_SECURE_NO_WARNINGS 1
#include "morse.h"

int main(int argc, char** argv) {

	//printFile(argv[1]);

	FILE* file = fopen(argv[1], "r");
	if (!file) return 1;

	int i = 0;

	abr** tree = createNode('\0');

	char sMorse[6];
	char car;
	while (fscanf(file, "%s %c\n", sMorse, &car) != EOF) {
		addNode(tree, sMorse, car);
	}

	fclose(file);

	//affTree(*tree);

	printf("Entrez un code morse : ");
	while (sMorse != EOF)
	{

		scanf("%s", sMorse);
		printf("%c", findChar(tree, sMorse));


	}

	return 0;

	//for (i = 0; i < 36; i++) {
	//	printf("line %d : %s %c\n", i, (&morse[0])[i], tab[i]);
	//}

}