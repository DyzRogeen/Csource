#pragma once
#include <functional>
#include <time.h>

class NeuralNetwork {

	LayerList* layerList = NULL;
	int size = 0;

public:
	NeuralNetwork(){};
	int getSize() { return this->size; };
	int setSize(int size) { this->size = size; };
	LayerList* getLayers() { return this->layerList; };
	NeuralNetwork addLayer(int size, std::function<float(float)> activation);
	void init();
	void setInputValues(float* vals);
	void propagate();
	void clear();
};

class Layer {

	int size;
	float bias;
	float* nodeVals;
	float* W_mat;
	std::function<float(float)> activation;

public:
	Layer(int size, std::function<float(float)> activation);
	int getSize() { return this->size; }
	void setValues(float* vals);
	float* getValues() { return this->nodeVals; }
	float* getWeights() { return this->getWeights(); }
	void init(int size_next);
	void propagate(int size_next, float* vals_next);
	void clear();

};

typedef struct LayerListS {
	Layer layer;
	struct LayerListS* next;
	struct LayerListS* prec;
}LayerList;

float ReLu(float x);

LayerList* createList(Layer l);
void addLayerToList(LayerList** ll, Layer l);
void freeLayerList(LayerList* l);