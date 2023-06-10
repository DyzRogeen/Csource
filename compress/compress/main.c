#define _CRT_SECURE_NO_WARNINGS
#include <stdlib.h>
#include <stdio.h>
#include "comp.h"

int main(int argv, char** argc) {

	FILE* fichier = fopen("./rle.txt", "r");

	if (!fichier) {
		printf("Fichier non trouve.\n");
		return 0;
	}

	char c;
	char *s = (char*)calloc(75,sizeof(char));
	char *tmp = s;
	int i = 0;

	while ((c = fgetc(fichier)) != EOF) {

		if (c == '\n') {
			s[i] = '\0';
			//printf("%s\n", RLE(s));
			i = -1;
		}
		else {
			s[i] = c;
		}
		i++;

	}

	fclose(fichier);

	free(s);
	
	//for (i = 0; i < 48;i++) printf("%d : '%c' |", i, i);

	return 1;
}