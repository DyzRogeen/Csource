#pragma once

#include "utils_SDL.h"

typedef struct sPoint {
	int x;
	int y;
}point;

typedef struct sGraph {
	float* vector;
	int sideSize;
	int width, height;
}graph;

graph* initGraph(int resolution, int width, int height);

void printGraph(graph* g);

void affGraph(SDL_Surface* window, graph* g);

void applicatePressure(graph* g, int x, int y);

void colorCase(SDL_Surface* window, int x, int y, int width, int height, Uint8 level);

void resetGraph(SDL_Surface* window, graph* g);

int dist(float x1, float y1, float x2, float y2);

int readV(graph* g, int nLine);

void registerV(graph* g, int number);

void freeGraph(graph* g);
