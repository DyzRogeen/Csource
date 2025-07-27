#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include "graph.h"
#include "neuralNet.h"
#include "convolNet.h"
#include "../SDL2_image-2.6.3/include/SDL_image.h"
#include "../SDL2-2.28.1/include/SDL.h"

#define PI	3.14159265359

void step(NeuralNetwork* n, int numIter) {
	float x = rand() % 201 - 100;
	float y = rand() % 201 - 100;

	setInputValues(n, (float[2]) { x, y });
	propagate(n);
	printNetwork(n);
	retroPropagate(n, (float[1]) { 6 * x + 12 > y ? 1 : -1 });
	//printf("===AFTER RETRO===\n");
	//printNetwork(n);

	printf("\nJe ve %d", 6 * x + 12 > y ? 1 : -1);
	printf("\n\n");

}

int countNbWindows(CNNLayer* l) {
	int cnt = l->nbDatas;
	while (l) {
		cnt *= l->nbFilters;
		l = l->next;
	}
	return cnt;
}

int countFlattenSize(CNNLayer* l, int w, int h) {
	if (!l) return w * h;
	if (l->type == INITIAL) return countFlattenSize(l->next, w, h);
	
	CNNLayer* l_next = l->next;
	int cnt = 0;
	if (l->type == CONVOLUTIONAL) {
		sFilter* filter = l->filters;
		int minus;
		while (filter) {
			minus = filter->filter_size - 1;
			cnt += countFlattenSize(l_next, (w - minus) / filter->stride, (h - minus) / filter->stride);
			filter = filter->next;
		}
	}
	else if (l->type == POOLING)
		cnt += countFlattenSize(l_next, w / l->pooling_size, h / l->pooling_size);
	return cnt;
}

void renderSurface(SDL_Renderer* renderer, SDL_Surface* surface) {
	SDL_RenderClear(renderer);
	SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
	SDL_RenderCopy(renderer, texture, NULL, NULL);
	SDL_RenderPresent(renderer);
	SDL_DestroyTexture(texture);
}

void mapImg(SDL_Surface* win, SDL_Surface* img) {

	int w = win->w, h = win->h, bpp = img->format->BytesPerPixel;
	Uint32* pxls = win->pixels;
	Uint8* img_pxls = img->pixels;
	for (int i = 0; i < h * w; i++) {
		*(pxls++) = (0xFF000000 | img_pxls[0] << 16 | img_pxls[1] << 8 | img_pxls[2]);
		img_pxls += bpp;
	}

}

void mapDataOnSurface(SDL_Surface* s, sData dR, sData dG, sData dB) {
	int w = dR.w, h = dR.h, s_w = s->w, slot;
	int pxl_size = s_w / w;
	float* dataR = dR.data;
	float* dataG = dG.data;
	float* dataB = dB.data;
	Uint32* pxls = s->pixels, pxl;

	for (int i = 0; i < w * h; i++) {
		pxl = 0xFF000000 | (Uint8)max(0, 255 * min(1, dataR[i])) << 16 | (Uint8)max(0, 255 * min(1, dataG[i])) << 8 | (Uint8)max(0, 255 * min(1, dataB[i]));
		//pxl = 0xFF000000 | (Uint8)max(0, min(255, dataR[i])) << 16 | (Uint8)max(0, min(255, dataG[i])) << 8 | (Uint8)max(0, min(255, dataB[i]));
		//pxls[i] = dataToPxl(dataR[i], dataG[i], dataB[i]);
		slot = (i % w + (i / w) * s_w) * pxl_size;
		for (int y = 0; y < pxl_size; y++)
			for (int x = 0; x < pxl_size; x++)
				pxls[slot + x + y * s_w] = pxl;

	}


}

Uint32 dataToPxl(float R, float G, float B) {
	Uint8 r = max(0, min(255, R));
	Uint8 g = max(0, min(255, G));
	Uint8 b = max(0, min(255, B));
	return 0xFF000000 | r << 16 | g << 8 | b;
}

sData* getRGBDataFromPxls(SDL_Surface* win) {
	int w = win->w, h = win->h;
	int length = w * h;
	Uint8* pxl = win->pixels;

	float* dataR = (float*)calloc(length, sizeof(float));
	float* dataG = (float*)calloc(length, sizeof(float));
	float* dataB = (float*)calloc(length, sizeof(float));
	for (int i = 0; i < length; i++) {
		dataR[i] = pxl[2];
		dataG[i] = pxl[1];
		dataB[i] = pxl[0];
		pxl+=4;
	}

	sData* D = (sData*)malloc(3 * sizeof(sData));
	D[0] = createCNNData(dataR, h, w);
	D[1] = createCNNData(dataG, h, w);
	D[2] = createCNNData(dataB, h, w);

	return D;
}

sData* getDataFromPxls(SDL_Surface* win) {
	int w = win->w, h = win->h;
	int length = w * h;
	Uint8* pxl = win->pixels;

	float* data = (float*)calloc(length, sizeof(float));
	for (int i = 0; i < length; i++) {
		data[i] = (pxl[2] + pxl[1] + pxl[0]) / 3;
		pxl += 4;
	}

	sData* D = (sData*)malloc(sizeof(sData));
	*D = createCNNData(data, h, w);

	return D;
}

SDL_Surface** displayDatas(sData* D, int nbDataPerCanal, int RGBmode) {

	SDL_Surface** windows = (SDL_Surface**)calloc(nbDataPerCanal, sizeof(SDL_Surface*)), * window, * surface;
	SDL_Renderer* renderer;
	SDL_Texture* texture;
	sData dR, dG, dB;

	int forced_w = 100, forced_h;

	if (RGBmode) nbDataPerCanal /= 3;

	int dx = 0;
	for (int i = 0; i < nbDataPerCanal; i++) {
		dR = D[i];
		
		if (RGBmode) {
			dG = D[i + nbDataPerCanal];
			dB = D[i + 2 * nbDataPerCanal];
		}

		if (dR.w > 50) {
			forced_w = dR.w;
			forced_h = dR.h;
		}
		else {
			forced_w = 100;
			forced_h = forced_w * dR.h / dR.w;
		}

		window = SDL_CreateWindow("Neural Network", dx, 20, forced_w, forced_h, SDL_WINDOW_SHOWN);
		surface = SDL_CreateRGBSurface(0, forced_w, forced_h, 32, 0x00FF0000, 0x0000FF00, 0x000000FF, 0xFF000000);
		renderer = SDL_CreateRenderer(window, 0, 0);

		dx += 30;

		if (RGBmode) mapDataOnSurface(surface, dR, dG, dB);
		else		 mapDataOnSurface(surface, dR, dR, dR);
		renderSurface(renderer, surface);
		
		SDL_FreeSurface(surface);
		SDL_DestroyRenderer(renderer);

		windows[i] = window;

	}

	//free(D);

	return windows;

}

SDL_Surface** convol(SDL_Surface* win, CNNLayer* l, int RGBmode) {

	sData* D, *Dres, *Dtmp, *Dtmp2;
	int nbData, init = 1;

	if (RGBmode) {
		D = getRGBDataFromPxls(win);
		nbData = 3;
	} else {
		D = getDataFromPxls(win);
		nbData = 1;
	}

	l->datas = D;
	l->nbDatas = nbData;
	l = l->next;

	while (l) {

		switch(l->type) {
		case CONVOLUTIONAL:

			Dres = (sData*)malloc(nbData * l->nbFilters * sizeof(sData));
			Dtmp = Dtmp2 = Dres;

			for (int i = 0; i < nbData; i++) {
				Dtmp = applyConvolution(D[i], l);
				for (int j = 0; j < l->nbFilters; j++) Dres[j] = Dtmp[j];
				Dres += l->nbFilters;
			}
			nbData *= l->nbFilters;

			// Nettoyage des données si existantes
			if (l->datas != NULL) {
				Dtmp = l->datas;
				for (int i = 0; i < l->nbDatas; i++) free(Dtmp[i].data);
				free(Dtmp);
			}

			l->datas = Dtmp2;
			l->nbDatas = nbData;

			// Free initial D
			//if (0 && init) {
			//	free(D[0].data);
			//	free(D[0].L);
			//	if (RGBmode) {
			//		free(D[1].data);
			//		free(D[1].L);
			//		free(D[2].data);
			//		free(D[2].L);
			//	}
			//	init = 0;
			//	free(D);
			//}

			D = Dtmp2;

			break;
		case POOLING:
			Dres = (sData*)malloc(nbData * sizeof(sData));
			for (int i = 0; i < nbData; i++) Dres[i] = maxPool(D[i], l->pooling_size);

			// Nettoyage des données si existantes
			if (l->datas != NULL) {
				for (int i = 0; i < l->nbDatas; i++) {
					free(l->datas[i].data);
					free(l->datas[i].maxIndexes);
				}
				free(l->datas);
			}

			l->datas = D = Dres;
			l->nbDatas = nbData;
			break;
		default:
			break;
		}

		l = l->next;
	}

	return displayDatas(D, nbData, RGBmode);

}

SDL_Surface** CNNStep(SDL_Surface* win, CNNLayer* CNN, NeuralNetwork* N, int nbIterations, int RGBmode) {

	// TODO : prendre une autre image à chaque fois
	SDL_Surface** imgs = NULL;

	for (int i = 0; i < nbIterations; i++) {
		// Propagation CNN
		imgs = convol(win, CNN, RGBmode);
		// Applatissage vers partie fully-connected
		flatten(CNN, N);
		// Propagation Réseau Dense
		propagate(N);

		// RétroPropagation Réseau Dense
		retroPropagate(N, (float[1]) {-1}); // TODO : Remplacer 1 si chat, -1 si chien
		// Rétropropagation des pertes Réseau Dense vers CNN
		mapLoss(CNN, N);
		// Rétropropagation CNN
		retroPropagateCNN(CNN);

	}

	return imgs;

}

int main(int argc, char **argv)
{
	SDL_Surface * window, *img, *CNNview, ** windows = NULL;
	SDL_Surface* surface, *CNNSurface;
	SDL_Renderer* renderer, *CNNRenderer;
	SDL_Texture* texture;
	int quit = 0;
	int nbWindows, RGBmode = 1;
	SDL_Event e;
	point mousePos;

	srand(time(NULL));

	// Filtres de Sobel
	float filterH[25] = { 
		1, 1, 0, -1, -1,
		1, 1, 0, -1, -1,
		5, 1, 0, -1, -5,
		1, 1, 0, -1, -1,
		1, 1, 0, -1, -1,
	};
	float filterW[25] = {
		 1,  1,  5,  1,  1,
		 1,  1,  1,  1,  1,
		 0,  0,  0,  0,  0,
		-1, -1, -1, -1, -1,
		-1, -1, -5, -1, -1,
	};
	// Filtre Laplacien
	float filterC[9] = {
		0, 1, 0,
		1,-4, 1,
		0, 1, 0,
	};
	// Filtres d'Emboss
	float filterE1[9] = {
		-2,-1, 0,
		-1, 1, 1,
		 0, 1, 2,
	};
	float filterE2[9] = {
		 0,-1,-2,
		 1, 1,-1,
		 2, 1, 0,
	};
	float filterE3[9] = {
		 2, 1, 0,
		 1, 1,-1,
		 0,-1,-2,
	};
	float filterE4[9] = {
		 0, 1, 2,
		-1, 1, 1,
		-2,-1, 0,
	};

	if (!(img = IMG_Load("../dataset/10006.jpg"))) {
		printf("Unable to open file.\n");
		exit(EXIT_FAILURE);
	}

	// Init CNN
	CNNLayer* CNN = createInitCNN();

	CNNLayer* l = createConvolutionalLayer(createDynamicFilter(5, 3), ReLu, dReLu);
	addFilter(l, createDynamicFilter(5, 3));
	addFilter(l, createDynamicFilter(3, 2));
	addFilter(l, createDynamicFilter(3, 2));
	addCNNLayer(CNN, l);

	addCNNLayer(CNN, createPoolingLayer(MAX, 2));

	CNNLayer* l2 = createConvolutionalLayer(createDynamicFilter(5, 3), ReLu, dReLu);
	addFilter(l2, createDynamicFilter(5, 3));
	addFilter(l2, createDynamicFilter(5, 3));
	addFilter(l2, createDynamicFilter(3, 2));
	addFilter(l2, createDynamicFilter(3, 2));
	addFilter(l2, createDynamicFilter(3, 2));
	addCNNLayer(CNN, l2);

	addCNNLayer(CNN, createPoolingLayer(MAX, 2));

	int flatten_size = countFlattenSize(CNN, img->w, img->h) * (RGBmode ? 3 : 1);

	// Init Réseau Dense
	NeuralNetwork* neuralNetwork = createNetwork();
	addLayer(neuralNetwork, flatten_size, ReLu, dReLu);
	addLayer(neuralNetwork, 24, ReLu, dReLu);
	addLayer(neuralNetwork, 18, ReLu, dReLu);
	addLayer(neuralNetwork, 1, ReLu, dReLu);

	init(neuralNetwork);

	// Init Fenêtre Réseau Dense
	if (SDL_Init(SDL_INIT_VIDEO) < 0) {
		fprintf(stderr, "ERREUR - SDL_Init\n>>> %s\n", SDL_GetError());
		exit(EXIT_FAILURE);
	}

	atexit(SDL_Quit);
	window = SDL_CreateWindow("Neural Network", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 640, 480, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
	if (window == NULL) {
		fprintf(stderr, "ERREUR - impossible de passer en : %dx%dx%d\n>>> %s\n", 640, 480, 32, SDL_GetError());
		exit(EXIT_FAILURE);
	}
	surface = SDL_CreateRGBSurface(0, 640, 480, 32, 0x00FF0000, 0x0000FF00, 0x000000FF, 0xFF000000);
	renderer = SDL_CreateRenderer(window, 0, 0);

	drawNetwork(surface, neuralNetwork);
	renderSurface(renderer, surface);

	// Affichage Image d'origine
	CNNview = SDL_CreateWindow("CNNview", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, img->w, img->h, SDL_WINDOW_SHOWN);
	CNNSurface = SDL_CreateRGBSurface(0, img->w, img->h, 32, 0x00FF0000, 0x0000FF00, 0x000000FF, 0xFF000000);
	CNNRenderer = SDL_CreateRenderer(CNNview, 0, 0);
	mapImg(CNNSurface, img);
	renderSurface(CNNRenderer, CNNSurface);

	//windows = convol(CNNSurface, CNN, 0);

	while (!quit)
	{
		while (SDL_PollEvent(&e))
		{
			switch (e.type)
			{
			case SDL_QUIT:
				quit = 1;
				break;
			case SDL_KEYDOWN:
				if (e.key.keysym.sym == SDLK_ESCAPE)
					quit = 1;
				if (e.key.keysym.sym == SDLK_s) {
					windows = CNNStep(CNNSurface, CNN, neuralNetwork, 1, RGBmode);
					drawNetwork(surface, neuralNetwork);
					renderSurface(renderer, surface);

					break;
					for (int i = 0; i < 5; i++) {
						step(neuralNetwork, 1);
						drawNetwork(surface, neuralNetwork);
						renderSurface(renderer, surface);
					}
					
				}

				break;
			case SDL_MOUSEBUTTONDOWN:
				SDL_GetMouseState(&mousePos.x, &mousePos.y);
				break;
			}
		}

		if (0)
		{
			drawNetwork(surface, neuralNetwork);
			renderSurface(renderer, surface);

		}
	}

	nbWindows = countNbWindows(CNN);
	for (int i = 0; i < nbWindows; i++)
		SDL_DestroyWindow(windows[i]);
	free(windows);

	clear(neuralNetwork);
	freeCNNLayer(CNN);
	SDL_FreeSurface(surface);
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);

	SDL_FreeSurface(CNNSurface);
	SDL_DestroyRenderer(CNNRenderer);
	SDL_DestroyWindow(CNNview);

	return EXIT_SUCCESS;
}