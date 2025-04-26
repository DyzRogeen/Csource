#include "mot.h"
#include "plan.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

float fnc(int x, int y) {
	return rand() % 21 - 10;
}

plan* createPlan(int width, int height, int res, point3 pos) {
	plan* pl = (plan*)calloc(1, sizeof(plan));

	pos.x -= width * res / 2;
	pos.z -= height * res / 2;

	srand(time(NULL));

	point3** points = (point3**)calloc(width * height, sizeof(point3*));
	for (int i = 0; i < width * height; i++) {
		points[i] = createPoint(pos.x + res * (i % width), pos.y + fnc(10 - i % width, 10 - i / width), pos.z + res * (i / width));
		if (i != 0) points[i - 1]->next = points[i];
	}

	pl->points = points;
	pl->width = width;
	pl->height = height;

}

obj* getObjFromPlan(plan* pl) {
	point3** points = pl->points;
	int w = pl->width, h = pl->height;
	face* f = NULL;
	
	for (int x = 0; x < w - 1; x++)
		for (int y = 0; y < h - 1; y++) {
			addFace(&f, createFace(WHITE, 0, 3, points[x + y * w], points[x + y * w + 1], points[x + (y + 1) * w]));
			addFace(&f, createFace(WHITE, 1, 3, points[x + y * w], points[x + y * w + 1], points[x + (y + 1) * w]));
			addFace(&f, createFace(WHITE, 0, 3, points[x + y * w + 1], points[x + (y + 1) * w + 1], points[x + (y + 1) * w]));
			addFace(&f, createFace(WHITE, 1, 3, points[x + y * w + 1], points[x + (y + 1) * w + 1], points[x + (y + 1) * w]));
		}

	return createObj3D(f, points[0], (w - 1) * (h - 1) * 4, w * h, 1, 0);
}

void freePlan(plan* pl) {
	freePoints(pl->points[0]);
	free(pl);
}