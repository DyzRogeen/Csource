#pragma once

#define PI	3.14159265359

typedef struct sPoint {
	int x;
	int y;
	struct sPoint* next;
}point;

typedef struct sFace {
	int nbPoints;
	point normale;
	point* points;
}face;

//Vector Basic Operations

point sum(point p1, point p2);

point scale(point p, int n);

//Faces methods

face createFace(point n);

face addPointFace(face f, point* p);

int isInPlane(face f, point* p);