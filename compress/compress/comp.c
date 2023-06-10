#include <stdlib.h>
#include <stdio.h>
#include "comp.h"

char* RLE(char* s) {

	int i = 0, j = 0, k = 0;
	char c;
	char* s1 = (char*)calloc(75, sizeof(char));

	while (s[k] != '\0') {

		s1[j] = '$';
		c = s[k];
		for (i = k; s[i] != '\0' && s[i] == c; i++);
		if (i - k > 3) {
			s1[++j] = i - k;
			j++;
		}
		else
			i = k + 1;
		s1[j++] = c;
		k = i;
	}
	s1[j + 1] = '\0';
	return s1;
}

list* count(char* s) {

	int tab[50];

	for (int i = 0; i < 50; i++) tab[i] = 0;

	for (int i = 0; s[i] != '\0'; i++) {
		tab[s[i]]++;
	}

}