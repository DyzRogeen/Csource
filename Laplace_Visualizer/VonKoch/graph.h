#pragma once
#include "../../Common_Libs/SDL-1.2.15/include/SDL.h"
#include <math.h>
#include <stdlib.h>

#define WHITE	0x00FFFFFF
#define DGREY	0x00333333
#define BLUE	0x000000FF
#define GREEN	0x0000AA00
#define RED		0x00AA0000
#define ORANGE	0x00993300

#define E 2.71828182846

typedef struct sPoint {
	int x, y;
}point;
typedef struct sPointf {
	float x, y;
}pointf;

typedef struct sGraph {
	Uint32* pixels;
	int width, height;
	float scaleX, scaleY;
	point offset;
	float zoom;
}Graph;

void drawGrid(Graph* g);

void drawCurve(Graph* g, point p_mouse, int tMax);

void drawLine(Uint32* pxls, int w, int h, pointf p1, pointf p2, Uint32 color);

pointf func(float t);

pointf dotMExp(pointf pFunc, pointf p, float t);

// UTILS

pointf getCoordinates(point p, int ratioX, int ratioY, int offsetX, int offsetY);
pointf getGraphCoordinates(pointf p, int ratioX, int ratioY, int offsetX, int offsetY);

pointf addPoint(pointf p1, pointf p2, int diff);
pointf scale(pointf v, float scale);
pointf orthonormal(pointf v);
float norm(pointf v);