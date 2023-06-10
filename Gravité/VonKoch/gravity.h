#pragma once

#define PI	3.14159265359

typedef struct sPoint {
	float x;
	float y;
}point;

typedef struct sEntity {
	point position;
	point direction;
	float mass;
	float radius;
	char* name;
}entity;

entity* CreateEntity(point posistion, point direction, float mass, float radius, char* name);

void AddEntity(entity** entityList, entity* e);

float Norm(point v);

point Sum(point v1, point v2, int sign);

point Scale(point v, float s);

point Vect(point v1, point v2);

entity* EntityByName(entity** entityList, char* name);

void SimulOnce(SDL_Surface* window, entity** entityList, int showVectors, int nbE);

void DisplayEntities(SDL_Surface* window, entity** entityList, int showVectors, int nbE);

float SetPixelIntensity(point origin, point p);

void Drawircle(SDL_Surface* window, entity* e);