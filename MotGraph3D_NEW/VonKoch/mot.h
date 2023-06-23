#pragma once

#include "utils_SDL.h"
#define PI	3.14159265359

typedef struct sPoint {
	int x;
	int y;
	int display;
}point;

typedef struct sPoint3 {
	float x,y,z;
	point p;
	struct sPoint3* next;
}point3;

typedef struct sEdge {
	point3* points[2];
	struct sEdge* next;
}edge;

typedef struct sFace {
	int nbPoints;
	point3 normale;
	point3 G;
	point3* points;
	struct sFace* next;
}face;

typedef struct sObj3 {
	int nbFaces, nbEdges, nbVertexes;
	face* faces;
	edge* edges;
	point3* vertexes;
}obj;

typedef struct sCam {
	point3 pos, pCam;
	point3 dir, vUnit1, vUnit2;
	float lon, lat;
	float dist;
}cam;

typedef struct sLight {
	float intensity;
	point3 pos;
}light;

//Utils

void printP3(point3 p);

//Vector Basic Operations

// 2D
point sum2(point p1, point p2, int diff);

float norm2(point p);

// 3D
point3 sum(point3 p1, point3 p2, int diff);

void add(point3* p1, point3 p2, int diff);

point3 scale(point3 p, float n);

float norm(point3 p);

//Points methods

point3* createPoint(float x, float y, float z);

point3 setPoint(float x, float y, float z);

//Faces and Edges methods

edge* createEdge(point3* p1, point3* p2);

face* createFace(point3 n, point3* p, int nbPoints);

void printFace(face f);

void addPointFace(face *f, point3* p);

int isInPlane(face f, point3* p);

obj* createObj3D(face* f, edge* e, point3* p, int nbFaces, int nbEdges, int nbVertexes);

//Graphic methods

cam initCam(point3 pos, point3 dir);

void moveCam(cam* C, point3 pos);

void rotateCam(cam* C, float lon, float lat);

void pointsTo2dProjection(int screenW, int screenH, point3* p, int nbPoints, cam c);

void colorTriangle(SDL_Surface* p_affichage, point p1, point p2, point p3, const Uint32 color);

void colorFace(SDL_Surface* window, point3* p, int nbPoints, const Uint8 r, const Uint8 g, const Uint8 b);

//Modelisation methods

//face* ExtrudeFace(face f, point3 dir, float l);