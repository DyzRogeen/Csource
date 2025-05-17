#pragma once

#include <math.h>
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
	pointf* v;
	pointf* p;
	struct sListP* next;
}listP;

typedef struct sObj {
	listP* points;
	int nbPoints;
	pointf* v;
	// vitesse de rot à 1pxl du pRot
	float vRot;
	pointf pRot;
	Uint32 color;
	int isStatic;
	float mass;
}obj;

typedef struct sListO {
	obj* o;
	struct sListO* next;
}listO;

pointf* createPoint(float x, float y);

listP* createList(pointf* p);

void addPoint(listP** l, pointf* p);

obj* createObj(listP* points, Uint32 color, float mass, int isStatic);

listO* createListO(obj* o);

void addObj(listO** O, obj* o);

void verifyOffScreen(SDL_Surface* window, listO* O);

void colorRow(SDL_Surface* window, float x1, float x2, float y, const Uint32 color);

void colorTriangle(SDL_Surface* window, pointf p1, pointf p2, pointf p3, const Uint32 color);

void colorObj(SDL_Surface* window, listP* lP, int nbPoints, Uint32 color);

void displayVector(SDL_Surface* window, obj* o, pointf p, int scale);

void movePoint(obj* o, pointf* p);

//void test(SDL_Surface* window, obj* o, listP* pl);

void contact(obj* o1, obj* o2, pointf pc1, pointf p1segc2, pointf p2segc2);

void seekCollision(listO* O);

void collide(obj* o1, obj* o2);

void handleCollision(obj* o1, obj* o2, pointf p_contact);

void getSegContact(pointf p, pointf pn, listP* P, float* x1, float* y1, float* x2, float* y2);

// UTILS

pointf sum(pointf p1, pointf p2, int diff);

pointf scale(pointf v, float scale);

pointf setPoint(float x, float y);

float projectionCoef(pointf v1, pointf v2);

pointf orthogonal(pointf v);

pointf unit(pointf v);

float norm(pointf v);

pointf rot2v(obj o, pointf p);

float longestRange(obj* o);

void projSeg(pointf p, pointf v, listP* P, float* segMin, float* segMax);

// PRINT

void printPnt(pointf p);

void printListP(listP* P);

void printObj(obj* o);

// FREE

void freeListP(listP* P);

void freeObj(obj* o);

void freeListO(listO* O);