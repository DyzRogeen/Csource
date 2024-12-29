#pragma once

#include "mot.h"

typedef struct sPlan {

	int width, height;
	point3** points;

}plan;

float fnc(int x, int y);

plan* createPlan(int width, int height, int res, point3 pos);

obj* getObjFromPlan(plan* pl);

void freePlan(plan* pl);