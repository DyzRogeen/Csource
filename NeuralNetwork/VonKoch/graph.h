#pragma once
#include <math.h>
#include "utils_SDL.h"
#include "neuralNet.h"

#define WHITE 1 << 25 - 1
#define RED 255 << 16
#define BLUE 255

typedef struct sPointf {
	float x, y;
} pointf;

typedef struct sPoint {
	int x, y;
} point;

typedef struct sNeuron {
	point p;
	float val;
}neuron;

void drawNeuron(SDL_Surface* window, point p, float val, int size);

void drawLink(SDL_Surface* window, point p1, point p2, int size, float w);

void drawNetwork(SDL_Surface* window, NeuralNetwork n);

int getMaxLayerSize(LayerList* ll);


pointf unit(pointf v);

pointf scale(pointf v, float f);

point sumPoint(point p1, point p2, int diff);

pointf sumPointf(pointf p1, pointf p2, int diff);

point setPoint(int x, int y);

pointf setPointf(float x, float y);

float norm(pointf v);