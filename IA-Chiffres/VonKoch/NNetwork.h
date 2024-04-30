#pragma once
#include <stdio.h>
#include <stdlib.h>

typedef enum eLayerType {
	INPUT,
	HIDDEN,
	OUTPUT
}layerType;

typedef enum eLayerPos {
	FIRST,
	INT,
	LAST
}layerPos;

typedef struct sNeuron {
	float val;
	float* weight;
}neuron;

typedef struct sLayer {
	neuron** neurons;
	int size;
	layerType type;
}layer;

typedef struct sNetwork {
	layer** layers;
	int nbLayers;
}network;

typedef struct sWeight {
	float** W;
	float* nVal;
	int nbLayers;
	int layerSize;
	layerPos position;
}weight;

network *initNetwork(int* layerSizes, int nbLayers);

weight** initWeight(int* layerSizes, int nbLayers);

void learn(weight** w);

float* propagation(weight** w);

float* retroPropagation(weight** w, int number);

void guess(weight** w, float* v, int vectorSize, int verbose);

void stats(weight** w);

void normalize(float* layer, int layerSize);

/*void freeNetwork(network* nN);

void freeLayer(layer* l);

void freeNeuron(neuron* n);*/

void freeWeight(weight** w);

int vton(int* v);

float* ntov(int n);

int maxIndex(float* v, int verbose);

int* findNewline(FILE* f, int nbLines);

float sigmoid(float v);