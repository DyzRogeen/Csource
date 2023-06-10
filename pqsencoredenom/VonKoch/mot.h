#pragma once

typedef struct sPoint {
	int x;
	int y;
}point;

typedef struct sRect {
	point p1, p2;
}rect;

typedef struct sList {
	rect obj;
	struct sList* next;
}list;


list* createList(rect obj);

void addNode(list* l, rect obj);

void printList(list* l);

rect createRect(point p1, point p2);

void displayRect(SDL_Surface* window, list* l);

void freeList(list* l);