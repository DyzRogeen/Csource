#pragma once
#include "utils_SDL.h"

typedef enum{STATIC, DYNAMIC}stat;

typedef enum{LEFT, RIGHT, UP, DOWN}dep;

typedef struct sPoint {
	int m_x;
	int m_y;
}point;

typedef struct sBloc{

	point a;
	point b;
	stat s;

}bloc;

typedef struct sList {

	bloc* rect;
	struct sList* next;

}list;

bloc* createBloc(point a, point b, stat s);

list* createList(bloc* rect);

void insertList(list** l, bloc* rect);

list* popList(list* l);

void printList(list* l);

void affBloc(bloc* rect, SDL_Surface* window);

void affAll(list* l, SDL_Surface* window);

void moveBloc(list* l, dep d, float* dx, float* dy);

int canMoveBloc(list* l, bloc* rect, dep d, float* dx, float* dy);

list** isDynamic(list* l);

void moveAll(list* l);//TODO

void freeList(list *l);