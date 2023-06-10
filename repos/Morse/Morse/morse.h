#pragma once

typedef struct ABR {

	char m_data;
	struct ABR* tab[2];

}abr;

enum{LEFT, RIGHT};

void affTree(abr* tree);

void printFile(const char* argv);

abr* createNode(char car);

abr** findNode(abr** tree, char* morse);

void addNode(abr** tree, char* morse, char car);

char findChar(abr* tree, char* morse);

#define _CRT_SECURE_NO_WARNINGS 1
