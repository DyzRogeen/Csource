#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include "utils_SDL.h"
#include "mot.h"

list* createList(rect obj) {
	list* l = (list*)calloc(1, sizeof(list));
	l->obj = obj;
	return l;
}

void addNode(list** l, rect obj) {
	if (!&l || !l) {
		*l = createList(obj);
		return;
	}

	list* tmp = createList(obj);
	tmp->next = *l;
	*l = tmp;
}

void printList(list* l) {
	if (!l) {
		printf("\n");
		return;
	}

	printf("[(%d;%d):(%d;%d)] => ", l->obj.p1.x, l->obj.p1.y, l->obj.p2.x, l->obj.p2.y);
	printList(l->next);
}

rect createRect(point p1, point p2) {
	rect obj;
	obj.p1 = p1;
	obj.p2 = p2;
	return obj;
}

void displayRect(SDL_Surface* window, list* l) {
	if (!l) return;
	_SDL_DrawLine(window, l->obj.p1.x, l->obj.p1.y, l->obj.p2.x, l->obj.p1.y, 255,255,255);
	_SDL_DrawLine(window, l->obj.p1.x, l->obj.p2.y, l->obj.p2.x, l->obj.p2.y, 255,255,255);
	_SDL_DrawLine(window, l->obj.p1.x, l->obj.p1.y, l->obj.p1.x, l->obj.p2.y, 255,255,255);
	_SDL_DrawLine(window, l->obj.p2.x, l->obj.p1.y, l->obj.p2.x, l->obj.p2.y, 255,255,255);

	displayRect(window, l->next);
}

void freeList(list* l) {
	if (!l) return;
	freeList(l->next);
}