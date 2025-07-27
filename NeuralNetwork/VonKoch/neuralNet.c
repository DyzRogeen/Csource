#include <stdio.h>
#include "neuralNet.h"

// Neural Network

NeuralNetwork* createNetwork() {
	NeuralNetwork* n = (NeuralNetwork*)calloc(1, sizeof(NeuralNetwork));
	n->layerList = NULL;
	n->size = 0;
	return n;
}

void addLayer(NeuralNetwork* n, int size, float (*activation)(float), float (*deriv)(float)) {
	layer* l = createLayer(size, activation, deriv);
	addLayerToList(n, &n->layerList, l);
	n->size++;
}

void init(NeuralNetwork* n) {
	LayerList* ll = n->layerList;
	while (ll->next) {
		initLayer(ll->layer, ll->next->layer->size);
		ll = ll->next;
	}
}

void setInputValues(NeuralNetwork* n, float* vals) {
	layer* l = n->layerList->layer;
	float* v = l->nodeVals;
	int size = l->size;

	for (int i = 0; i < size; i++) {
		v[i] = vals[i];
	}
}

void initOutputErrors(layer* l, float* t) {
	float* e = l->e, * h = l->h, *y = l->nodeVals, size = l->size;
	float (*deriv)(float) = l->deriv;

	for (int i = 0; i < size; i++) e[i] = deriv(h[i]) * (y[i] - t[i]);

}

void propagate(NeuralNetwork* n) {
	LayerList* ll = n->layerList;
	while (ll->next) {
		propagateLayer(ll->layer, ll->next->layer);
		ll = ll->next;
	}
	printf("%f\n", *ll->layer->nodeVals);
}

void retroPropagate(NeuralNetwork* n, float* t) {

	LayerList* ll = n->lastLayer;
	initOutputErrors(ll->layer, t);
	while (ll->prec) {
		retroPropagateLayer(ll->prec->layer, ll->layer);
		ll = ll->prec;
	}
	                                   
}

void clear(NeuralNetwork* n) {
	freeLayerList(n->layerList);
	free(n);
}

// Layer

layer* createLayer(int size, float (*activation)(float), float (*deriv)(float)) {

	layer* l = (layer*)calloc(1, sizeof(layer));

	l->size = size;
	l->activation = activation;
	l->deriv = deriv;
	l->h = (float*)calloc(size, sizeof(float));
	l->e = (float*)calloc(size, sizeof(float));
	l->nodeVals = (float*)calloc(size, sizeof(float));
	l->W_mat = NULL;

	return l;
}

void initLayer(layer* l, int size_next) {

	srand(time(NULL)); // Génération aléatoire dans [-1/sqrt(M);1/sqrt(M)];

	int size = l->size + 1; // +1 for the bias
	float* W = (float*)calloc(size * size_next, sizeof(float));

	float sqrtM = 100*sqrtf(size_next);

	for (int in = 0; in < size; in++)
		for (int out = 0; out < size_next; out++)
			*(W + in + out * size) = (float)(rand() % 201 - 100) / sqrtM;

	l->W_mat = W;

}

void setValues(layer* l, float* vals) {
	float* nodeVals = l->nodeVals;
	for (int i = 0; i < l->size; i++) nodeVals[i] = vals[i];
}

void propagateLayer(layer* l, layer* l_next) {
	int size = l->size, size_next = l_next->size;
	float* vals = l->nodeVals, * vals_next = l_next->nodeVals, *h_next = l_next->h;
	float* W = l->W_mat;
	float sum, mean = 0;

	float (*activation)(float) = l->activation;

	// Pour chaque neurone de sortie ...
	for (int out = 0; out < size_next; out++) {
		sum = 0;
		// On somme les produits entre neurones d'entrée et leur poids
		for (int in = 0; in < size; in++) {
			sum += vals[in] * W[in + out * size];
		}
		sum += W[size * (1 + out)]; // Bias

		h_next[out] = sum;
		mean += vals_next[out] = (out == size_next - 1) ? sum : activation(sum);
	}
	mean /= size_next;

	// Centrer et réduire la sortie
	if (size_next > 1) normalize(vals_next, size_next, mean);
}

void retroPropagateLayer(layer* l_prec, layer* l) {
	int size = l->size, size_prec = l_prec->size;
	float* vals = l->nodeVals, *h = l->h, *e = l->e;
	float* vals_prec = l_prec->nodeVals, * h_prec = l_prec->h, * e_prec = l_prec->e;
	float* W_prec = l_prec->W_mat, sum;

	float (*deriv)(float) = l->deriv, (*deriv_prec)(float) = l_prec->deriv;
	float pas = 0.02;
	
	int slot = 0;
	for (int in = 0; in < size_prec; in++) {
		sum = 0;
		for (int out = 0; out < size; out++) {
			sum += W_prec[slot] * e[out];
			W_prec[slot] -= pas * e[out] * vals_prec[in];
			slot++;
		}
		// Propagation de l'erreur : ej = g'(hj) * S[ wij * ei ]
		e_prec[in] = deriv_prec(h_prec[in]) * sum;
	}

	// Pour tous les poids de la dernière colonne (biais) on met à jour les biais
	for (int out = 0; out < size; out++) W_prec[size_prec * (1 + out)] -= pas * e[out]; // Bias
}

void clearLayer(layer* l) {
	free(l->h);
	free(l->e);
	free(l->nodeVals);
	free(l->W_mat);
	free(l);
}

// FONCTIONS D'ACTIVATION

float ReLu(float x) {
	return x;// x < 0 ? 0 : x;
}
float dReLu(float x) {
	return 1;//x < 0 ? 0 : 1;
}

float sigmoid(float x) {
	return 1.f / (1 + powf(E, -x));
}
float dsigmoid(float x) {
	return powf(E, -x) / powf(1 + powf(E, -x), 2);
}

// UTILS

void normalize(float* y, int N, float mean) {
	float std = 0;

	for (int i = 0; i < N; i++)
		std += (y[i] - mean) * (y[i] - mean);

	if (std == 0) return;
	
	std = sqrtf(std / N);

	for (int i = 0; i < N; i++)
		y[i] = (y[i] - mean) / std;

}

void printNetwork(NeuralNetwork* n) {

	printf("---------------------------------\nSize : %d\n", n->size);
	LayerList* ll = n->layerList;

	while (ll) {
		printLayer(ll->layer, ll->next ? ll->next->layer->size : 0);
		ll = ll->next;
	}

}
void printLayer(layer* l, int size_next) {
	float* v = l->nodeVals;
	int size = l->size;
	printf("[ ");
	for (int i = 0; i < size; i++) printf("%.2f ", v[i]);
	if (size_next == 0) printf("]\n\n");
	else				printf("| 1 ]\n\n");

	float* w = l->W_mat;
	for (int out = 0; out < size_next; out++) {
		printf("[[ ");
		for (int in = 0; in < size; in++) (w[in + out * size] < 0 ? printf("%.2f ", w[in + out * size]) : printf(" %.2f ", w[in + out * size]));
		printf("| %.2f ]]\n", w[size * (1 + out)]); // Bias vector
	}
	printf("\n");
}

LayerList* createList(layer* l) {
	LayerList* ll = (LayerList*)calloc(1, sizeof(LayerList));
	ll->prec = NULL;
	ll->next = NULL;
	ll->layer = l;

	return ll;
}

void addLayerToList(NeuralNetwork* n, LayerList** ll, layer* l) {
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
	n->lastLayer = tmp;
}

void freeLayerList(LayerList* l) {
	if (!l) return;
	if (l->next) freeLayerList(l->next);
	clearLayer(l->layer);
}