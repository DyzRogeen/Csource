#pragma once

#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#define PI	3.14159265359

typedef enum eType { VCC, GND, GENERATEUR, RESISTANCE, BOBINE, CONDENSATEUR, DIODE, WIRE} type;

typedef struct sPoint {
	float x, y;
	float V; // Potentiel Electrique
	int alt; // Valeur s'il a été vérifié ou non
	struct sPoint* pnext_Connect;
	struct sPoint* pprec_Connect;
	struct sElec* e;
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

void initSimulation(list* l, int alt);
int propagateV(point* p, int alt);
int propagateForward(point* p, int alt);
int propagateBackward(point* p, int alt);