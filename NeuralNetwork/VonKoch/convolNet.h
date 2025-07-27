#pragma once

#include <math.h>
#include <time.h>
#include <stdio.h>
#include "neuralNet.h"

typedef enum POOLING_TYPE {AVERAGE, MIN, MAX} poolingType;
typedef enum LAYER_TYPE {INITIAL, CONVOLUTIONAL, POOLING} layerType;

typedef struct sData {
	float* L; // Loss
	float* data;
	int* maxIndexes;
	int w, h;
}sData;

typedef struct sFilter {
	float* filter;
	float bias;
	int filter_size;
	int stride;
	struct sFilter* next;
}sFilter;

typedef struct sCNNLayer {

	layerType type;
	struct sCNNLayer* next;
	struct sCNNLayer* prec;
	struct sCNNLayer* last;

	// Convolutional Layer
	sFilter* filters;
	int nbFilters;
	float (*activation)(float);
	float (*deriv)(float);

	// Pooling Layer
	poolingType pool_type;
	int pooling_size;

	sData* datas;
	int nbDatas;

}CNNLayer;

sData applyFilter(sData D, sFilter F, float(*activation)(float));

sData* applyConvolution(sData D, CNNLayer* l);

sData maxPool(sData D, int pooling_size);

void flatten(CNNLayer* l, NeuralNetwork* n);

void mapLoss(CNNLayer* L, NeuralNetwork* n);

void retroPropagateCNN(CNNLayer* l);

void retroPropagateConvolution(CNNLayer* l);

void retroPropagatePooling(CNNLayer* l);

void updateFilter(sFilter* f, sData D_in, sData D_out);

void updateLoss(sFilter* f, sData D, sData D_prec, float(*deriv)(float), int isFirstUpdate);

// UTILS

sData createCNNData(float* data, int height, int width);

sFilter* createfilter(float* filter, int filter_size, int stride);

sFilter* createDynamicFilter(int filter_size, int stride);

void addFilter(CNNLayer* l, sFilter* f);

CNNLayer* createInitCNN();

CNNLayer* createConvolutionalLayer(sFilter* f, float (*activation)(float), float (*deriv)(float));

CNNLayer* createPoolingLayer(poolingType type, int size);

void addCNNLayer(CNNLayer* L, CNNLayer* l);

void freeFilters(sFilter* F);

void freeCNNLayer(CNNLayer* L);