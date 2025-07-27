#pragma once
#include <time.h>
#include <math.h>

#define E 2.71828

typedef struct sLayer {

	int size;
	float* h; // Avant la fonction d'activation (utile dans la rétropropadation pour éviter de le recalculer)
	float* e; // Erreur
	float* nodeVals;
	float* W_mat;
	float (*activation)(float);
	float (*deriv)(float);

}layer;

typedef struct LayerListS {
	layer* layer;
	struct LayerListS* next;
	struct LayerListS* prec;
}LayerList;

typedef struct sNeuralNetwork {

	LayerList* layerList;
	LayerList* lastLayer;
	int size;

}NeuralNetwork;

float ReLu(float x);
float dReLu(float x);
float sigmoid(float x);
float dsigmoid(float x);

NeuralNetwork* createNetwork();
void addLayer(NeuralNetwork* n, int size, float (*activation)(float), float (*deriv)(float));
void init(NeuralNetwork* n);
void setInputValues(NeuralNetwork* n, float* vals);
void initOutputErrors(layer* l, float* t);
void propagate(NeuralNetwork* n);
void retroPropagate(NeuralNetwork* n, float* t);
void clear(NeuralNetwork* n);

layer* createLayer(int size, float (*activation)(float), float (*deriv)(float));
void initLayer(layer* l, int size_next);
void setValues(layer* l, float* vals);
void propagateLayer(layer* l, layer* l_next);
void retroPropagateLayer(layer* l_prec, layer* l);
void clearLayer(layer* l);

void normalize(float* y, int N, float mean);
void printNetwork(NeuralNetwork* n);
void printLayer(layer* l, int size_next);
LayerList* createList(layer* l);
void addLayerToList(NeuralNetwork* n, LayerList** ll, layer* l);
void freeLayerList(LayerList* l);