#define _CRT_SECURE_NO_WARNINGS 1

#include "pile.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void polon(char* token) {
	pile* pInt = (pile*)malloc(sizeof(pile));
	while (token) {
		if (!(strcmp(token, "+"))) push(&pInt, pop(&pInt) + pop(&pInt));
		else if (!(strcmp(token, "-"))) push(&pInt, -1 * pop(&pInt) + pop(&pInt));
		else if (!(strcmp(token, "*"))) push(&pInt, pop(&pInt) * pop(&pInt));
		else if (!(strcmp(token, "/"))) {
			int tmp = pop(&pInt);
			push(&pInt, pop(&pInt) / tmp);
		}
		else { push(&pInt, atoi(token)); }
		printf("%s\n", token);
		token = strtok(NULL, " ");
	}printf("Résultat = %d\n", pInt->info);
	free(pInt);
}

void polon(char* token) {

	int i = 0;
	int par = 0;
	while (token[i] != '\0') {
		if (token[i] == '(') par++;
		if (token[i] == ')') par--;
		i++;
	}if (par != 0) return;

	pile* pInt = (pile*)malloc(sizeof(pile));
	pile* pOpe = (pile*)malloc(sizeof(pile));

	while (token) {
		if (!(strcmp(token, "+")) || !(strcmp(token, "-")) || !(strcmp(token, "*")) || !(strcmp(token, "/"))) push(&pOpe, token);
		else if (!(strcmp(token, ")"))) {
			int val1 = pop(&pInt);
			int val2 = pop(&pInt);
			swith(pop(&pOpe)) {
				case "+": push(&pOpe, val1 + val2); break;
				case "-": push(&pOpe, val2 - val1); break;
				case "*": push(&pOpe, val1 * val2); break;
				case "/": push(&pOpe, val2 / val1);
			}
		}
		else if (strcmp(token, "(")) { push(&pInt, atoi(token)); }
		printf("%s\n", token);
		token = strtok(NULL, " ");
	}printf("Résultat = %d\n", pOpe->info);

	free(pInt);
	free(pOpe);

}

void main() {

	char exp[1024];
	scanf("%[^\n]", exp);
	char* token = strtok(exp, " ");
	//polon(token);
	infix(exp);
	
}