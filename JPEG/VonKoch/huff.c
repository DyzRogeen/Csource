#define _CRT_SECURE_NO_WARNINGS 0

#include "huff.h"
#include "utils_SDL.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

tree* createTree(int val, char c) {

	tree* t = (tree*)calloc(1, sizeof(tree));
	//tree** children = (tree**)calloc(2, sizeof(tree*));
	//*(t->children) = children;
	t->c = c;
	t->val = val;
	return t;

}

char seekChar(tree* root, char* path) {

	if (!root) return "";
	if (!*path) return root->c;

	return seekChar(root->children[(*path != '0')], path + 1);

}

void printTree(tree* root) {

	if (!root) return;

	char c0, c1;
	int val0, val1;
	tree** children = root->children;

	if (children[0]) {
		c0 = children[0]->c;
		val0 = children[0]->val;
	}
	else {
		c0 = "";
		val0 = -1;
	}
	if (children[1]) {
		c1 = children[1]->c;
		val1 = children[1]->val;
	}
	else {
		c1 = "";
		val1 = -1;
	}


	printf("%c:%d => (%c:%d , %c:%d)\n", root->c, root->val, c0, val0, c1, val1);
	printTree(root->children[0]);
	printTree(root->children[1]);

}

void freeTree(tree* root) {

	if (!root) return;
	// tention aussi
	freeTree(root->children[0]);
	freeTree(root->children[1]);
	//free(root->children);
	free(root);

}

int charCount(int* t) {

	FILE* f = fopen("compressed.txt", "rb");

	int cnt = 0;
	char c;
	while ((c = getc(f)) != '\0') {
		t[c + (c < 0) * 256]++;
		cnt++;
	}

	fclose(f);
	return cnt;

}

int getCouples(int* t, int* tchar, int charCnt) {

	int cnt = 0;

	for (int i = 1; i < 256; i++) if (t[i]) {
		tchar[cnt++] = i;
		tchar[cnt++] = t[i];
	}

	return cnt;

}

void sort(int* t, int nbElem) {

	int valTmp, cTmp;

	for (int i = 1; i < nbElem; i+=2)
		for (int j = i + 2; j < nbElem; j+=2) {

			if (t[i] > t[j]) {
				valTmp = t[i];
				cTmp = t[i - 1];

				t[i] = t[j];
				t[i - 1] = t[j - 1];

				t[j] = valTmp;
				t[j - 1] = cTmp;

			}

		}

}

tree* mkTree(int* t, int nbElem) {

	tree* root, *tmp;
	tree** treeTab = (tree**)calloc(nbElem, sizeof(tree*));

	for (int i = 0; i < nbElem; i += 2) treeTab[i / 2] = createTree(t[i + 1], t[i]);
	nbElem /= 2;

	int modifNbElem;

	while (nbElem > 1) {

		modifNbElem = 0;

		for (int i = 0; i < nbElem; i += 2) {

			if (nbElem - i < 2) {
				treeTab[i / 2] = treeTab[i];
				continue;
			}

			tmp = createTree(treeTab[i]->val + treeTab[i + 1]->val, "");
			tmp->children[0] = treeTab[i];
			tmp->children[1] = treeTab[i + 1];

			treeTab[i/2] = tmp;
			modifNbElem--;

		}

		nbElem += modifNbElem;

	}
	root = treeTab[0];
	//printTree(root);

	free(treeTab);

	return root;

}

char* lookForChar(char c, tree* t, int depth, char* num) {

	if (t->c == c) return num;
	if (t->c != '0' || depth == 0) return "3";
	
	char* s = (char*)calloc(depth, sizeof(char));
	strcat(s, lookForChar(c, t->children[0], depth - 1, "0"));
	if (*s != '3') {
		strcat(s, num);
		return s;
	}

	*s = '\0';

	strcat(s, lookForChar(c, t->children[1], depth - 1, "1"));
	if (*s != '3') {
		strcat(s, num);
		return s;
	}

	return s;

}

int length(char* s) {
	int i;
	for (i = 0; s[i]; i++);
	return i;
}

void strreverse(char* s) {

	char tmp;
	int l = length(s);
	int l2 = l / 2;
	for (int i = 0; i < l2; i++) {
		tmp = s[i];
		s[i] = s[l - i - 1];
		s[l - i - 1] = tmp;
	}

}

void getCode(char** s, tree* t, int nbChar, int* tchar) {

	char c;
	int depth = roundf(log2(nbChar) + 0.49);

	for (int i = 0; i < nbChar; i++) {

		c = tchar[2 * i];
		s[c] = lookForChar(c, t, depth, "\0");
		strreverse(s[c]);
		printf("(%d)%c => %s\n", c, c, s[c]);

	}

}

void encodeHuff(char** coded, int nbChar, int* tchar) {

	Uint8 c = 0;
	char* code, chr;
	int bCnt = 0;

	FILE* f = fopen("huffman.txt", "w");
	FILE* f2 = fopen("compressed.txt", "rb");
	FILE* f3 = fopen("duffman2.txt", "w");
	int tmp = -1;

	// WRITING HEADER
	fprintf(f, "%d ", nbChar);
	for (int i = 0; i < nbChar; i+=2) fprintf(f, "%c %d ", tchar[i], tchar[i + 1]);

	// ENCODING DATA
	while ((chr = getc(f2)) != '\0') {
		putc(chr, f3);
		tmp++;
		code = coded[chr];
		while (*code && bCnt < 8) {
			c = c << 1;
			c |= (*code == '0' ? 0 : 1);
			bCnt++;
			code++;
			if (bCnt == 8) {
				putc(c, f);
				//printf("(%d)%c ", c, c);
				c = 0;
				bCnt = 0;
			}
		}
	}
	// COMPLETING LAST BYTE
	if (bCnt != 0) {
		c = c << (8 - bCnt);
		putc(c, f);
		//printf("(%d)%c ", c, c);
	}
	putc('\0', f);
	putc('\0', f);

	printf("\n");

	fclose(f);
	fclose(f2);
	fclose(f3);

}

void decodeHuff() {

	FILE* f = fopen("huffman.txt", "rb");
	FILE* f2 = fopen("compressed2.txt", "w");
	FILE* f3 = fopen("duffman2.txt", "w");

	// READING HEADER
	int nbChar;
	fscanf(f, "%d ", &nbChar);

	// BUILDING TREE
	int* tchar = (int*)calloc(nbChar, sizeof(int));
	for (int i = 0; i < nbChar; i += 2) {
		tchar[i] = getc(f);
		fscanf(f, " %d", &(tchar[i + 1])); getc(f);
		printf("[(%d)%c %d] => ", tchar[i], tchar[i], tchar[i + 1]);
	}printf("\n\n");
	
	tree* t = mkTree(tchar, nbChar);

	// READING DATA
	char c, *code = (char*)calloc(8, sizeof(char)), decoded;
	*code = '\0';
	int bCnt;
	while (1) {

		if ((c = getc(f)) == '\0') {
			if (getc(f) == '\0') break;
			else fseek(f, -1, SEEK_CUR);
		}
		if (c == '\r') {
			if (getc(f) == '\n') c = '\n';
			else fseek(f, -1, SEEK_CUR);
		}
		putc(c, f3);
		//printf("(%d)%c ",c, c);

		bCnt = 7;

		while (bCnt != -1) {
			strcat(code, ((c & (1 << bCnt)) >> bCnt) ? "1" : "0");
			if ((decoded = seekChar(t, code)) != '0') {
				putc(decoded, f2);
				*code = '\0';
			}
			bCnt--;
		}

	}
	putc('\0', f2);

	fclose(f);
	fclose(f2);
	fclose(f3);
	freeTree(t);

}

void huffComp() {

	int t[256];

	for (int i = 0; i < 256; i++) t[i] = 0;

	int charCnt = charCount(t);
	int* tchar = (int*)calloc(2 * charCnt, sizeof(int));

	charCnt = getCouples(t, tchar, charCnt);

	sort(tchar, charCnt);

	for (int i = 0; i < charCnt; i += 2) printf("[(%d)%c %d] => ", tchar[i], tchar[i], tchar[i + 1]); printf("\n\n");

	tree* codeTree = mkTree(tchar, charCnt);

	char* coded[256];

	getCode(coded, codeTree, charCnt / 2, tchar);

	//for (int i = 0; i < 19; i++) printf("%s", coded[s[i]]); printf("\n");

	encodeHuff(coded, charCnt, tchar);

	//for (int c = 1; c < 256; c++) free(coded[c]);
	
	free(tchar);
	freeTree(codeTree);

	decodeHuff();

}