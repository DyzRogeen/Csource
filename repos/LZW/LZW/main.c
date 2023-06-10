#define _CRT_SECURE_NO_WARNINGS
#include "fileBin.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

void code(FILE* f) {

	int taille = 1;
	char** dico = (char**)calloc(1, sizeof(char*));
	char lu[2];
	char* tampon = (char*)calloc(10, sizeof(char));
	char* tmp2 = (char*)calloc(10, sizeof(char));
	char* tmp = (char*)calloc(10, sizeof(char));
	int index;

	while (f) {
		lu[0] = fgetc(f);
		lu[1] = '\0';
		tmp2 = tampon;
		tmp = strcat(tampon, lu);
		tampon = tmp2;
		//printf("%s : %s : %s\n", tmp, tampon, lu);
		if ((index = estDansDico(dico, tmp, taille))!=-1 || tmp[1] == '\0')tampon = tmp;
		else {
			printf("2\n");
			if ((index = estDansDico(dico, tampon, taille)) != -1) printf("%s", dico[index]);
			else printf("%s,,,,", tampon);
				
			dico[taille - 1] = tmp;
			taille++;
			realloc(dico, taille*sizeof(char*));
			strcpy(tampon, lu);
		}
		printf("3\n");
	}
	printf("%s", dico[index]);
}

int estDansDico(char** tab, char* str, int taille) {

	printf("OUI\n");
	for (int i = 0; i < taille-1; i++) {
		printf("indice %d : %s\n", i, tab[i]);
		if (strcmp(tab[i], str)) return i;
	}
	return -1;

}

int main(int argv, char** argc) {

	FILE* f = fopen("./Texte.txt", "r");

	if (!f) {

		printf("ERREUR : Le fichier n'a pas pu être ouvert.\n");
		return 1;

	}

	code(f);
}