#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include "utils_SDL.h"
#include "gravity.h"

entity* CreateEntity(point position, point direction, float mass, float radius, char*  name) {
	
	entity* e = (entity*)calloc(1, sizeof(entity));
	e->position.x = position.x;
	e->position.y = position.y;
	e->direction.x = direction.x;
	e->direction.y = direction.y;
	e->mass = mass;
	e->radius = radius;
	e->name = name;
	return e;

}

void AddEntity(entity** entityList, entity* e) {

	//if (!entityList) return;
	if (!*entityList)
		entityList = (entity**)calloc(1, sizeof(entity*));
	else
		entityList = (entity**)realloc(entityList, sizeof(entityList) + sizeof(entity*));
	
	int index;
	for (index = 0; entityList[index]; index++);
	printf("%d\n", index);

	entityList[index] = e;

}

float Norm(point v) {

	return sqrtf(v.x*v.x + v.y*v.y);

}

point Sum(point v1, point v2, int sign) {

	v1.x += v2.x * sign;
	v1.y += v2.y * sign;
	return v1;

}

point Scale(point v, float s) {

	v.x *= s;
	v.y *= s;
	return v;

}

point Vect(point v1, point v2) {

	//printf("(%f,%f) | (%f,%f) => %f\n", v1.x, v1.y, v2.x, v2.y, (v1.x * v2.x + v1.y * v2.y) / (v2.x * v2.x + v2.y * v2.y));
	return Scale(v2, (v1.x * v2.x + v1.y * v2.y) / (v2.x * v2.x + v2.y * v2.y));

}

entity* EntityByName(entity** entityList, char* name) {
	
	for (int i = 0; entityList[i]; i++)
		if (strcmp(entityList[i]->name, name))
			return entityList[i];
	return NULL;

}

void SimulOnce(SDL_Surface* window, entity** entityList, int showVectors, int nbE) {

	point dir;
	float dist;
	float scale;
	entity* e1;
	entity* e2;

	for (int i = 0; i < nbE; i++) {

		e1 = entityList[i];
		point sum = e1->direction;

		for (int j = 0; j < nbE; j++) {

			if (i == j) continue;

			e2 = entityList[j];

			dir = Sum(e2->position, e1->position, -1);

			dist = Norm(dir);

			dir = Scale(dir, 1 / dist);

			if (dist < e1->radius + e2->radius) {
				sum = Sum(sum, Scale(Vect(e1->direction, dir),e2->mass/(e1->mass*e1->mass)), -1);
				entityList[j]->direction = Sum(e2->direction,Scale(Vect(e1->direction, dir), e1->mass / (e2->mass*e2->mass)), 1);
			}
			else {
				scale = e2->mass / pow(dist, 2);
				sum = Sum(sum, Scale(dir, scale), 1);
			}

		}

		entityList[i]->direction = sum;
		entityList[i]->position = Sum(e1->position, Scale(sum, 1/e1->mass), 1);

	}

	DisplayEntities(window, entityList, showVectors, nbE);

}

void DisplayEntities(SDL_Surface* window, entity** entityList, int showVectors, int nbE) {

	entity* e;
	point seg;
	Uint32 coul = SDL_MapRGB(window->format, 255, 255, 255);

	for (int i = 0; i < nbE; i++) {
		
		e = entityList[i];

		if (showVectors) {
			seg = Sum(e->position, e->direction, 5.f);
			_SDL_DrawLine(window, window->w / 2 + e->position.x, window->h / 2 - e->position.y, window->w / 2 + seg.x, window->h / 2 - seg.y, 255 * i / nbE, 255, 255 * i / nbE);
		}
		else {
			Drawircle(window, entityList[i]);
			//coul = SDL_MapRGB(window->format, 255, 255, 255);
			//_SDL_SetPixel(window, window->w/2 + round(e->position.x), window->h / 2 - round(e->position.y), coul);
		}

	}

}

float SetPixelIntensity(point origin, point p) {

	float x_rate, y_rate;
	int x_error = (p.x - origin.x >= 0);
	int y_error = (p.y - origin.y >= 0);
	x_rate = x_error + (p.x - trunc(p.x))*(x_error?1:-1);
	y_rate = y_error + (p.y - trunc(p.y))*(y_error?1:-1);
	return x_rate * y_rate;

}

void Drawircle(SDL_Surface* window, entity* e) {

	Uint32 coul = SDL_MapRGB(window->format, 255, 255, 255);
	float r;

	for (int i = -round(e->radius); i <= round(e->radius); i++) {

		r = sqrt(e->radius* e->radius - i*i);
		_SDL_DrawLine(window, window->w/2 + e->position.x + r, window->h/2 + e->position.y + i, window->w/2 + e->position.x - r, window->h/2 + e->position.y + i, 255, 255, 255);

	}

}