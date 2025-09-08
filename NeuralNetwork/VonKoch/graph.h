#pragma once
#include <math.h>
#include <stdio.h>
#include "neuralNet.h"
#include "../SDL2-2.28.1/include/SDL.h"


#define WHITE 255 << 16 | 255 << 8 | 255 | 255 << 24
#define RED 255 << 16 | 255 << 24
#define BLUE 255 | 255 << 24
#define max(a,b) a > b ? a : b
#define min(a,b) a < b ? a : b

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

void drawNeuron(SDL_Surface* window, int px, int py, float val, int size);

void drawLink(SDL_Surface* window, point p1, point p2, int size, float w);

void drawNetwork(SDL_Surface* window, NeuralNetwork* n);

void drawLayer(SDL_Surface* window, layer* l, layer* l_next, int x, int y, int size, int space, int margin);

int getMaxLayerSize(LayerList* ll, int maxSize);


pointf unit(pointf v);

pointf scale(pointf v, float f);

point sumPoint(point p1, point p2, int diff);

pointf sumPointf(pointf p1, pointf p2, int diff);

point setPoint(int x, int y);

pointf setPointf(float x, float y);

float norm(pointf v);