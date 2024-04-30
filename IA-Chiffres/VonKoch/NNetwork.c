#define _CRT_SECURE_NO_WARNINGS 0
#define e 2.71828182846

#include "NNetwork.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

network *initNetwork(int* layerSizes, int nbLayers) {

	network* nN = (network*)calloc(1, sizeof(network));
	layer** l_tab = (layer**)calloc(nbLayers, sizeof(layer*));

	layer* l;
	neuron** n;

	float* w;

	srand(time(NULL));

	for (int i = 0; i < nbLayers; i++) {

		l = (layer*)calloc(1, sizeof(layer));
		n = (neuron**)calloc(layerSizes[i], sizeof(neuron*));
		
		if (i < nbLayers - 1) {

			for (int j = 0; j < layerSizes[i]; j++) {

				w = (float*)calloc(layerSizes[i + 1], sizeof(float));

				for (int k = 0; k < layerSizes[i + 1]; k++) w[k] = (rand() % 200 - 100) / 100;

				n[j]->weight = w;

			}
			l->type = (i == 0) ? INPUT : HIDDEN;

		}
		else l->type = OUTPUT;

		l->size = layerSizes[i];
		l->neurons = n;
		
		l_tab[i] = l;

	}

	nN->nbLayers = nbLayers;
	nN->layers = l_tab;

	return nN;

}

weight** initWeight(int* layerSizes, int nbLayers) {
	
	weight *l, **w = (weight**)calloc(nbLayers, sizeof(weight*));
	float** W, *nVal, *n;
	int nbInputNeurons, nbOutputNeurons;

	srand(time(NULL));

	int i;
	for (i = 0; i < nbLayers - 1; i++) {

		l = (weight*)calloc(1, sizeof(weight));

		nbInputNeurons = layerSizes[i] + 1; // + 1 including bias
		nbOutputNeurons = layerSizes[i + 1];

		nVal = (float*)calloc(nbInputNeurons, sizeof(float));

		W = (float**)calloc(nbOutputNeurons, sizeof(float*));

		for (int j = 0; j < nbOutputNeurons; j++) {

			n = (float*)calloc(nbInputNeurons, sizeof(float));

			// Random Weights between -1/sqrt(M) and 1/sqrt(M)
			for (int k = 0; k < nbInputNeurons; k++) n[k] = ((float)(rand() % 201) / 100 - 1) / sqrt(nbInputNeurons);

			W[j] = n;

		}

		l->layerSize = layerSizes[i];
		l->nVal = nVal;
		l->W = W;
		l->nbLayers = nbLayers;
		l->position = (i == 0) ? FIRST : ((i == nbLayers - 2) ? LAST : INT);

		w[i] = l;

	}
	// For OUTPUT layer data
	l = (weight*)calloc(1, sizeof(weight));

	nVal = (float*)calloc(layerSizes[i], sizeof(float));
	l->nVal = nVal;
	l->layerSize = layerSizes[i];

	w[i] = l;

	return w;

}

void learn(weight** w) {
	
	FILE* db = fopen("database.txt", "r");

	int nbV, vectorSize, number;
	fscanf(db, "%d %d\n", &nbV, &vectorSize);

	int* nl = findNewline(db, nbV);

	int tmpV;
	float* tmpOut;

	// Pour toutes les lignes ...
	for (int i = 0; i < 10 * nbV; i++) {

		// Les lire dans un certain ordre.
		fseek(db, nl[(i * (nbV + 1) / 10) % nbV], SEEK_SET);

		// Lecture du vecteur et du chiffre représenté
		fscanf(db, "%d", &number);
		for (int j = 0; j < vectorSize; j++) {
			fscanf(db, " %d", &tmpV);
			w[0]->nVal[j] = tmpV / 100.0;
		}
		w[0]->nVal[vectorSize] = 0.; // Biais

		//printf("Reading line %d, number : %d\n", (i * (nbV + 1) / 10) % nbV, number);

		for (int i = 0; i < 20; i++) {
			// Propagation
			tmpOut = propagation(w);

			// Rétro-propagation
			retroPropagation(w, number);
		}
		//maxIndex(tmpOut, 1);

		//if (i != nbV - 1) fseek(db, 1, SEEK_CUR);

	}

	free(nl);
	fclose(db);

}

float* propagation(weight** w) {

	int layerSize_in = (*w)->layerSize;
	int layerSize_out = w[1]->layerSize - ((*w)->position != LAST); // On ne compte pas le neuronne du biais.
	float** W = (*w)->W;
	float* nVal_in = (*w)->nVal;
	float* nVal_out = w[1]->nVal;

	float sum;

	// Pour tous les neuronnes de sortie ...
	for (int i = 0; i < layerSize_out; i++) {
		
		// On normalise la couche d'entrée
		normalize(nVal_in, layerSize_in);

		sum = 0;
		// Somme du produit des poids avec les valeurs d'entrée ...
		for (int j = 0; j < layerSize_in; j++) {
			sum += W[i][j] * nVal_in[j];
			//if ((*w)->position == FIRST) printf("%f = %f * %f : sum = %f\n", W[i][j] * nVal_in[j], W[i][j], nVal_in[j], sum);
		}

		// Comme valeur des neuronnes de sorties.
		nVal_out[i] = sum;

	}

	w[1]->nVal = nVal_out;

	if ((*w)->position == LAST) {

		//maxIndex(nVal_out, 1);
		return nVal_out;
	}

	// Couche suivante.
	return propagation(w + 1);

}

float* retroPropagation(weight** w, int number) {

	float *error_current, *error_next = NULL, **W_next = NULL, delta;
	int layerSize_next;
	layerPos pos = (*w)->position;

	if (pos != LAST) {
		error_next = retroPropagation(w + 1, number);
		layerSize_next = w[2]->layerSize;
		W_next = w[1]->W;
	}

	int layerSize_in = (*w)->layerSize, layerSize_out = w[1]->layerSize;
	float** W = (*w)->W;
	float* nVal_in = (*w)->nVal, *nVal_out = w[1]->nVal;

	error_current = (float*)calloc(layerSize_out, sizeof(float));
	
	float pas = 0.0005;

	for (int i = 0; i < layerSize_in; i++) {

		// Dernière couche
		if (pos == LAST) for (int j = 0; j < layerSize_out; j++) {
			error_current[j] = nVal_out[j] - (j == number);
			W[j][i] -= pas * error_current[j] * nVal_in[i];
		}
		// Autres couches
		else for (int j = 0; j < layerSize_out; j++) {
			delta = 0;
			for (int k = 0; k < layerSize_next; k++) delta += W_next[k][j] * error_next[k];
			error_current[j] = delta;
			W[j][i] -= pas * delta * nVal_in[i];
		}
	}

	if (pos != LAST) free(error_next);

	return error_current;

}

void guess(weight** w, float* v, int vectorSize, int verbose) {

	for (int i = 0; i < vectorSize; i++) (*w)->nVal[i] = v[i];
	maxIndex(propagation(w), verbose);

}

void stats(weight** w) {

	FILE* db = fopen("database.txt", "r");

	int nbV, vectorSize, number;
	fscanf(db, "%d %d\n", &nbV, &vectorSize);

	int* nl = findNewline(db, nbV);
	float scores[11] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
	int tmpV;

	for (int i = 0; i < nbV; i++) {

		fseek(db, nl[i], SEEK_SET);

		fscanf(db, "%d", &number);
		for (int j = 0; j < vectorSize; j++) {
			fscanf(db, " %d", &tmpV);
			w[0]->nVal[j] = tmpV / 100.0;
		}
		if (number == maxIndex(propagation(w), 0)) {
			scores[number] += 1000.0 / (float)nbV;
			scores[10] += 100.0 / (float)nbV;
		}
	}

	for (int i = 0; i < 10; i++) printf("%d : %.0f %% de reussite.\n", i, scores[i]);
	printf("Overall : %.0f %% de reussite.\n", scores[10]);

	free(nl);
	fclose(db);

}

void normalize(float* layer, int layerSize) {
	float mean = 0.;
	float std = 0.;

	// Mean and standard deviation of the array
	for (int i = 0; i < layerSize; i++) {
		mean += layer[i];
		std += layer[i] * layer[i];
	}
	mean /= layerSize;
	std = sqrtf(std / layerSize - mean * mean);

	// Normalisation xi' = (xi - µ) / s
	for (int i = 0; i < layerSize; i++) {
		layer[i] = (layer[i] - mean) / std;
	}
}

// UTILS

void freeWeight(weight** w) {

	if (!w || !*w) return;

	layerPos pos = (*w)->position;
	if (pos != LAST) freeWeight(w + 1);

	float** W = (*w)->W;
	int layerSize = (*w)->layerSize;

	if (W != NULL) {
		//for (int i = 0; i < layerSize; i++) if (W[i] != NULL) free(W[i]);
		free(W);
	}
	free((*w)->nVal);
	free(*w);

	if (pos == FIRST) free(w);

}

int vton(int* v) {
	for (int i = 0; i < 10; i++)
		if (v[i] == 1)
			return i;
	return -1;
}

float* ntov(int n) {
	float v[10] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
	v[n] = 1;
	return v;
}

int maxIndex(float* v, int verbose) {
	float max = *v;
	int index_max = 0;

	if (verbose) printf("[ %.3f", max);
	for (int i = 1; i < 10; i++) {
		if (verbose)printf(" %.3f", v[i]);
		if (v[i] > max) {
			max = v[i];
			index_max = i;
		}
	}
	if (verbose) printf(" ]\t: je dis %d\n", index_max);
	return index_max;
}

int* findNewline(FILE* f, int nbLines) {

	int i, *nl = (int*)calloc(nbLines, sizeof(int));

	char s[1024];
	for (i = 0; i < nbLines - 1; i++) {
		nl[i] = ftell(f);
		fscanf(f, "%[^\n] ", s);
	}
	nl[i] = ftell(f);

	return nl;

}

float sigmoid(float v) {
	return (1 - pow(e, -v)) / (1 + pow(e, -v));
}

/*float* propagation(weight* w, float* v) {

	float*** W = w->W;
	int* layerSizes = w->layerSizes;
	int nbLayers = w->nbLayers;
	
	float* input = v;
	float** l, * n;
	float *output;
	float sum;

	for (int i = 0; i < nbLayers - 1; i++) {

		l = W[i];

		output = (float*)calloc(layerSizes[i + 1], sizeof(float));

		for (int j = 0; j < layerSizes[i + 1]; j++) {

			n = l[j];
			sum = 0;
			for (int k = 0; k < layerSizes[i]; k++) sum += n[k] * input[k];
			output[j] = sum;

		}
		free(input);
		input = (float*)calloc(layerSizes[i + 1], sizeof(float));
		for (int j = 0; j < layerSizes[i + 1]; j++) input[j] = output[j];
		free(output);

	}

	sum = 0;
	for (int i = 0; i < layerSizes[nbLayers - 1]; i++) sum += input[i];
	for (int i = 0; i < layerSizes[nbLayers - 1]; i++) input[i] /= sum;

	return input;

}

void freeNetwork(network* nN) {

	layer** l_tab = nN->layers;
	int nbLayers = nN->nbLayers;

	for (int i = 0; i < nbLayers; i++) freeLayer(l_tab[i]);
	free(l_tab);
	free(nN);

}

void freeLayer(layer* l) {

	neuron** n_tab = l->neurons;
	int nbNeurons = l->size;

	for (int i = 0; i < nbNeurons; i++) freeNeuron(n_tab[i]);
	free(n_tab);
	free(l);

}

void freeNeuron(neuron* n) {
	free(n->weight);
	free(n);
}*/