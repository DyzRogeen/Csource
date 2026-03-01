#pragma once

#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#define PI	3.14159265359

typedef enum eType { VCC, GND, GENERATEUR, RESISTANCE, BOBINE, CONDENSATEUR, DIODE, WIRE} type;
typedef enum eStep { RESET, INIT_V, MNA, SET_VOLTAGE, SET_CURRENT } step;

typedef struct sPoint {
	float x, y;
	float V; // Potentiel Electrique
	int alt; // Valeur s'il a été vérifié ou non
	step step;
	struct sPoint* pnext_Connect;
	struct sPoint* pprec_Connect;
	struct sElec* e;
	int id; // Id pour la matrice MNA
}point;

typedef struct sElec {
	point *p1, *p2;
	float U, I, R, L, C, q, Freq, amplU;
	type t;
	int id;
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
void freeElec(elec* e);

//list* simulate(list* l);
void initV(list* l, int alt);
//void initI(list* l, int alt);
int propagateV(point* p, int alt);
//void propagateI(point* p, float I, int alt);
//void nodeSum(point* p, int alt);
float enterOrExitNode(point* p);
int propagate(point* p, int alt, int direction);
//int propagateBackward(point* p, int alt);

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

//////////////////
// Préliminaires
int countNodes(list* l);
void resetIds(list* l);
void setNodeId(point* p, int* nb_nodes, int increment);

// Traîtement
float* buildMNAMatrix(list* l, int nb_nodes);
void fillMNARow(point* p, float* M, float* F, int nb_nodes);
void handlePole(point* p, int id, float* row, float* F);

// Restitution
void setVoltage(list* l, float* F);
void setNodePotential(point* p, float V);

// UTILS
void printMatrix(float* M, int size);
float* matrixVectorProduct(float* M, float* v, int size);
float* inverse(float* M, int dim);
float det(float* M, int dim, int skip_row, int* skip_col, int skip_nb);

int isInArray(int val, int* array, int size);
void scaleMatrix(float* M, int size, float val);
//////////////////

int isBifurc(point* p);

void resetElecs(list* l);

point* neighborSwitch(point* p);
point* poleSwitch(point* p);

int cntConnectedPoles(point* p);
int cntElecList(list* l);
point* pnext(point* p);
point* pprec(point* p);