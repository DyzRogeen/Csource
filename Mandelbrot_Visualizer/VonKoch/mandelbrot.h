#pragma once

#include <math.h>
#include <stdio.h>

#define PI	3.14159265359

typedef struct sPoint {
	int x, y;
}point;
typedef struct sPointf {
	double x, y;
}pointf;

int iterateJulia(double zx, double zy, double cx, double cy, int nb_iter);
int iterateMandelbrot(double cx, double cy, int nb_iter);

int iterate(pointf z, pointf c, float power, int nb_iter, float roof);

pointf powZ(pointf z, float power);

// Utils

point sum(point p1, point p2, int diff);
pointf sumf(pointf p1, pointf p2, int diff);

pointf scale(point v, float f);
pointf scalef(pointf v, float f);

float norm(pointf v);
