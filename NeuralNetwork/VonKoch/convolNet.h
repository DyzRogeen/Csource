#pragma once

#include <math.h>
#include <time.h>
#include <stdio.h>
#include "neuralNet.h"
#include "../SDL2_image-2.6.3/include/SDL_image.h"
#include "../SDL2-2.28.1/include/SDL.h"

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

void convolFromIMG(SDL_Surface* win, CNNLayer* l, int RGBmode);

void convol(CNNLayer* l, int RGBmode);

void CNNStepTab(float* tab, int height, int width, CNNLayer* l, NeuralNetwork* N, int nbIterations, float* y);

void CNNStep(SDL_Surface* win, CNNLayer* CNN, NeuralNetwork* N, int nbIterations, int RGBmode, float* y);

void guessTab(float* tab, CNNLayer* CNN, NeuralNetwork* N);

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

int countNbWindows(CNNLayer* l);

int countFlattenSize(CNNLayer* l, int w, int h);

float mini(float a, float b);

float maxi(float a, float b);

// SDL UTILS

void renderSurface(SDL_Renderer* renderer, SDL_Surface* surface);

void mapImg(SDL_Surface* win, SDL_Surface* img);

void mapDataOnSurface(SDL_Surface* s, sData dR, sData dG, sData dB);

sData* getRGBDataFromPxls(SDL_Surface* win);

sData* getDataFromPxls(SDL_Surface* win);

SDL_Surface** displayDatas(sData* D, int nbDataPerCanal, int RGBmode);