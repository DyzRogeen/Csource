#pragma once

#include "utils_SDL.h"

#define PI	3.14159265359

typedef struct sPoint {
	int x;
	int y;
}point;

typedef struct sPointF {
	float x;
	float y;
}pointf;

typedef struct sListP {
	point* p;
	struct sListP* next;
}listP;

typedef struct sObj {
	listP* points;
	int nbPoints;
	struct sObj* next;
	Uint32 color;
	int isStatic;
	float mass;
	pointf velocity;
}obj;

typedef struct sListO {
	obj* o;
	struct sListO* next;
}listO;

point sum(point p1, point p2, int diff);

point* createPoint(int x, int y);

listP* createList(point* p);

void freeListP(listP* P);

void addPoint(listP** l, point* p);

obj* createObj(listP* points, Uint32 color, float mass, int isStatic);

listO* createListO(obj* o);

void addObj(listO** O, obj* o);

void colorRow(SDL_Surface* window, int x1, int x2, int y, const Uint32 color);

void colorTriangle(SDL_Surface* window, point p, point p2, point p3, const Uint32 color);

void colorObj(SDL_Surface* window, listP* lP, int nbPoints, Uint32 color);

void moveObj(obj* o);