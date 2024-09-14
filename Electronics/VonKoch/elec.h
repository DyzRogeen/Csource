#pragma once

#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#define PI	3.14159265359

typedef enum eType {GENERATEUR, RESISTANCE, BOBINE, CONDENSATEUR, DIODE, WIRE} type;

typedef struct sPoint {
	float x;
	float y;
	struct sPoint* pnext_Connect;
	struct sPoint* pprec_Connect;
}point;

typedef struct sElec {
	point *p1, *p2;
	float U, I;
	type t;
	int selected;
}elec;

typedef struct sList {
	elec* e;
	struct sList* next;
	struct sList* prec;
}list;

point* createPoint(list* l, point p);

elec* createElec(list* l, point p1, point p2, type t);

list* createList(elec* e);

void addList(list** l, elec* e);

void deleteSelected(list** l);

void connect(point* p, point* pv);

void disconnect(point* p);

void tryConnect(list* l, point* p);

int areConnected(point* p, point* pv);

void printList(list* l);

void freeList(list* l);