#include "neuralNet.h"

// Neural Network

NeuralNetwork NeuralNetwork::addLayer(int size, std::function<float(float)> activation) {
	Layer l = Layer(size, activation);
	addLayerToList(&this->layerList, l);
	this->size++;
	return *this;
}

void NeuralNetwork::init() {
	LayerList* ll = this->layerList;
	while (ll->next) {
		ll->layer.init(ll->next->layer.getSize());
		ll = ll->next;
	}
}

void NeuralNetwork::setInputValues(float* vals) {
	this->layerList->layer.setValues(vals);
}

void NeuralNetwork::propagate() {
	LayerList* ll = this->layerList;
	while (ll->next) {
		Layer l_next = ll->next->layer;
		ll->layer.propagate(l_next.getSize(), l_next.getValues());
		ll = ll->next;
	}
}

void  NeuralNetwork::clear() {
	freeLayerList(this->layerList);
}

// Layer

Layer::Layer(int size, std::function<float(float)> activation) {
	this->size = size;
	this->activation = activation;
	this->nodeVals = (float*)calloc(size, sizeof(float));
}

void Layer::init(int size_next) {

	srand(time(NULL)); // Génération aléatoire dans [-1/sqrt(M);1/sqrt(M)];

	int size = this->getSize();
	float* W = (float*)calloc(size * size_next, sizeof(float));

	float sqrtM = sqrtf(size_next);

	for (int in = 0; in < size; in++)
		for (int out = 0; out < size_next; out++)
			*(W + in + out * size) = (float)(rand() % 201 - 100) / sqrtM;

	this->W_mat = W;
	this->bias = (float)(rand() % 201 - 100) / sqrtM;

}

void Layer::setValues(float* vals) {
	float* nodeVals = this->nodeVals;
	for (int i = 0; i < this->size; i++) nodeVals[i] = vals[i];
}

void Layer::propagate(int size_next, float* vals_next) {
	int size = this->size;
	float* vals = this->nodeVals;
	float bias = this->bias;
	float* W = this->W_mat;
	float sum;

	std::function<float(float)> activation = this->activation;

	for (int out = 0; out < size_next; out++) {
		sum = bias;
		for (int in = 0; in < this->size; in++) {
			sum += vals[in] * W[in + out * size];
		}
		vals_next[out] = activation(sum);
	}
}

void Layer::clear() {
	free(this->nodeVals);
	free(this->W_mat);
}

// FONCTIONS D'ACTIVATION

float ReLu(float x) {
	return x < 0 ? 0 : x;
}

// UTILS

LayerList* createList(Layer l) {
	LayerList* ll = (LayerList*)calloc(1, sizeof(LayerList));
	ll->prec = NULL;
	ll->next = NULL;
	ll->layer = l;

	return ll;
}

void addLayerToList(LayerList** ll, Layer l) {
	if (!ll) return;

	if (!*ll) {
		*ll = createList(l);
		return;
	}

	LayerList* tmp = createList(l);

	LayerList* tmp2 = *ll;
	while (tmp2->next) tmp2 = tmp2->next;
	tmp->prec = tmp2;
	tmp2->next = tmp;
}

void freeLayerList(LayerList* l) {
	if (!l->next) freeLayerList(l->next);
	l->layer.clear();
	free(l);
}