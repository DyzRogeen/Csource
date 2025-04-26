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
	float U, I, R, L, C, q, Freq, amplU;
	type t;
	int selected;
}elec;

typedef struct sList {
	elec* e;
	struct sList* next;
	struct sList* prec;
}list;

point* createPoint(point p);
elec* createElec(point p1, point p2, type t);
list* createList(elec* e);
void addList(list** l, elec* e);
void fuseLists(list** l1, list* l2);

void deleteSelected(list** l);

void connect(point* p, point* pv);
void disconnect(point* p);
void tryConnect(list* l, point* p);
int areConnected(point* p, point* pv);

void printList(list* l);
void printElec(elec* e);
void freeList(list* l);

//list* simulate(list* l);
void initV(list* l, int alt);
//void initI(list* l, int alt);
int propagateV(point* p, int alt);
//void propagateI(point* p, float I, int alt);
//void nodeSum(point* p, int alt);
float enterOrExitNode(point* p);
int propagateForward(point* p, int alt);
int propagateBackward(point* p, int alt);

void simuI(list* l, int alt);
float getVeq(point* p, float V, float R, int alt);
float propI(point* p, float V, float R, int alt);
float propIF(point* p, float V, float R, int alt);
void fetchVR(point* p, float* V, float* R, int alt);
void propV(point* p);

list* makeDerivEqCirc(list* l, int alt);
float checkDeriv(point* p, elec* e, int alt, point* (*dir)(point*));
list* makeSerialEqCirc(list* l, int alt);
list* makeSerialREq(point* p, int alt);

int isBifurc(point* p);

void resetElecs(list* l);

point* neighborSwitch(point* p);
point* poleSwitch(point* p);

int cntConnectedPoles(point* p);
int cntElecList(list* l);
point* pnext(point* p);
point* pprec(point* p);