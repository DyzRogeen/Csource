#define _CRT_SECURE_NO_WARNINGS 0

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "graph.h"
#include "neuralNet.h"
#include "convolNet.h"
#include "../SDL2_image-2.6.3/include/SDL_image.h"
#include "../SDL2-2.28.1/include/SDL.h"

#define PI	3.14159265359
#define DRAWTAB_RESOLUTION 16
#define DRAWTAB_HEIGHT 512
#define DRAWTAB_WIDTH 512

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

void drawPixelTab(SDL_Surface* win, int x, int y, int ratio_w, int ratio_h, float val) {
	Uint32* pxls = win->pixels;

	int w = win->w;
	int x_start = x * ratio_w;
	int y_start = y * ratio_h;
	Uint8 comp = min(255, 255 * val);
	Uint32 pxl = 0xFF000000 | comp << 16 | comp << 8 | comp;

	for (int i = y_start; i < y_start + ratio_h; i++)
		for (int j = x_start; j < x_start + ratio_w; j++)
			*(pxls + j + i * w) = pxl;
}

float norme(float x1, float y1, float x2, float y2) {
	return sqrtf(powf(x2 - x1, 2) + powf(y2 - y1, 2));
}

float* ctov(int figure) {
	float* v = (float*)calloc(10, sizeof(float));
	v[figure] = 1;
	return v;
}

void drawOnTab(SDL_Surface* win, point pos, int resolution, float* tab) {
	int w = win->w, h = win->h;
	int posx = pos.x, posy = pos.y;

	int pixel_ratio_w = w / resolution;
	int pixel_ratio_h = h / resolution;
	float l_max = pixel_ratio_w;

	int x = posx / pixel_ratio_w;
	int y = posy / pixel_ratio_h;

	int x_start = x - ((posx % pixel_ratio_w) < pixel_ratio_w / 2), x_end = x_start + 2;
	int y_start = y - ((posy % pixel_ratio_h) < pixel_ratio_h / 2), y_end = y_start + 2;
	x_start = max(0, x_start); x_end = min(resolution, x_end);
	y_start = max(0, y_start); y_end = min(resolution, y_end);

	float tmpVal;
	int center_x, center_y;
	for (int i = y_start; i < y_end; i++) {
		center_y = (i + 0.5) * pixel_ratio_h;
		for (int j = x_start; j < x_end; j++) {
			center_x = (j + 0.5) * pixel_ratio_w;
			tab[j + i * resolution] = tmpVal = mini(1, maxi(tab[j + i * resolution], 1 - norme(center_x, center_y, posx, posy) / l_max));
			drawPixelTab(win, j, i, pixel_ratio_w, pixel_ratio_w, tmpVal);
		}
	}
		
}

void saveTab(float* tab, int figure) {
	FILE* f = fopen("database_chiffres2.txt", "a");
	fprintf(f, "%d,%d", DRAWTAB_RESOLUTION, figure);

	char buffer[1024] = "", tmp[3] = "";
	int cnt0 = 0;
	float tmpVal;

	// Compression des 0
	for (int i = 0; i < DRAWTAB_RESOLUTION * DRAWTAB_RESOLUTION; i++) {
		tmpVal = tab[i];
		if (tmpVal < 0.04) cnt0++;
		else {
			if (cnt0 == 1) strcat(buffer, " 0");
			else if (cnt0 > 1) {
				strcat(buffer, " 0|");
				_itoa(cnt0, tmp, 10);
				strcat(buffer, tmp);
			}
			strcat(buffer, " ");
			_itoa(tmpVal * 255, tmp, 16);
			strcat(buffer, tmp);
			cnt0 = 0;
		}
	}
	if (cnt0 == 1) strcat(buffer, " 0");
	else if (cnt0 > 1) {
		strcat(buffer, " 0|");
		_itoa(cnt0, tmp, 10);
		strcat(buffer, tmp);
	}

	fprintf(f, "%s\n", buffer);
	fclose(f);
}

int readLine(FILE* f, float* tab) {
	char tmpBuffer[10];

	int resolution, figure, tmp = 0, i;
	if(fscanf(f, "%d,%d", &resolution, &figure) == -1) return -1;
	for (i = 0; i < resolution * resolution; i++) {
		fscanf(f, " %31s", tmpBuffer);
		if (!strncmp(tmpBuffer, "0|", 2)) {
			tmp = atoi(tmpBuffer + 2);
			for (int j = 0; j < tmp; j++) tab[i++] = 0;
			i--;
		}
		else tab[i] = (float)(strtol(tmpBuffer, NULL, 16)) / 255;
	}

	return figure;
}

void learnTab(float* tab, int height, int width, CNNLayer* l, NeuralNetwork* N, int nbIterations) {
	FILE* f = fopen("database_chiffres2.txt", "r");

	sData* D = malloc(sizeof(sData));
	D[0] = createCNNData(tab, height, width);
	l->datas = D;
	l->nbDatas = 1;

	int cnt = 1;
	int figure = readLine(f, tab);
	printf("Learning line %d.\n", cnt++);
	CNNStep(NULL, l, N, nbIterations, 0, ctov(figure));


	char dump[1024];
	while (fscanf(f, "\n") != -1) {
		figure = readLine(f, tab);
		if (figure == -1) break;
		printf("Learning line %d.\n", cnt++);
		CNNStep(NULL, l, N, nbIterations, 0, ctov(figure));
	}
	fclose(f);

	printf("Learning done.\n");

}

void displayTab(SDL_Surface* win, float* tab) {
	int ratio_w = DRAWTAB_WIDTH / DRAWTAB_RESOLUTION;
	int ratio_h = DRAWTAB_HEIGHT / DRAWTAB_RESOLUTION;
	for (int y = 0; y < DRAWTAB_RESOLUTION; y++)
		for (int x = 0; x < DRAWTAB_RESOLUTION; x++)
			drawPixelTab(win, x, y, ratio_w, ratio_h, *(tab++));
}

void clearTab(SDL_Surface* s, SDL_Renderer* r, float* tab) {
	for (int i = 0; i < DRAWTAB_RESOLUTION * DRAWTAB_RESOLUTION; i++) tab[i] = 0;
	SDL_FillRect(s, &s->clip_rect, 0xFF000000);
	renderSurface(r, s);
}

int main(int argc, char **argv)
{
	SDL_Surface * window, *img, *CNNview, *drawTab;
	SDL_Surface* surface, *CNNSurface, *drawSurface;
	SDL_Renderer* renderer, *CNNRenderer, *drawRenderer;
	SDL_Texture* texture;
	int quit = 0;
	int RGBmode = 1, drawing = 0;
	SDL_Event e;
	point mousePos;

	srand(time(NULL));

	// Filtres statiques
	{
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
	}

	if (!(img = IMG_Load("../dataset/10006.jpg"))) {
		printf("Unable to open file.\n");
		exit(EXIT_FAILURE);
	}

	// Init CNN
	CNNLayer* CNN = createInitCNN();
	{

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

	}

	int flatten_size = countFlattenSize(CNN, img->w, img->h) * (RGBmode ? 3 : 1);

	// Init Réseau Dense
	NeuralNetwork* neuralNetwork = createNetwork();
	{
		addLayer(neuralNetwork, flatten_size, ReLu, dReLu);
		addLayer(neuralNetwork, 24, ReLu, dReLu);
		addLayer(neuralNetwork, 18, ReLu, dReLu);
		addLayer(neuralNetwork, 1, ReLu, dReLu);

		init(neuralNetwork);
	}

	// Init Fenêtre Réseau Dense
	{
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

		/*drawNetwork(surface, neuralNetwork);
		renderSurface(renderer, surface);*/
	}

	// Affichage Image d'origine
	{
		CNNview = SDL_CreateWindow("CNNview", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, img->w, img->h, SDL_WINDOW_SHOWN);
		CNNSurface = SDL_CreateRGBSurface(0, img->w, img->h, 32, 0x00FF0000, 0x0000FF00, 0x000000FF, 0xFF000000);
		CNNRenderer = SDL_CreateRenderer(CNNview, 0, 0);
		mapImg(CNNSurface, img);
		renderSurface(CNNRenderer, CNNSurface);
	}

	// Fenêtre de dessin
	float* tab = (float*)calloc(DRAWTAB_RESOLUTION * DRAWTAB_RESOLUTION, sizeof(float));
	{
		drawTab = SDL_CreateWindow("Draw Tab", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, DRAWTAB_WIDTH, DRAWTAB_HEIGHT, SDL_WINDOW_SHOWN);
		drawSurface = SDL_CreateRGBSurface(0, DRAWTAB_WIDTH, DRAWTAB_HEIGHT, 32, 0x00FF0000, 0x0000FF00, 0x000000FF, 0xFF000000);
		drawRenderer = SDL_CreateRenderer(drawTab, 0, 0);

		clearTab(drawSurface, drawRenderer, tab);
	}

	CNNLayer* CNNTab = createInitCNN();
	CNNLayer* l = createConvolutionalLayer(createDynamicFilter(3, 2), ReLu, dReLu);
	addFilter(l, createDynamicFilter(3, 2));
	addFilter(l, createDynamicFilter(3, 2));
	addFilter(l, createDynamicFilter(3, 2));
	addCNNLayer(CNNTab, l);

	CNNLayer* l2 = createConvolutionalLayer(createDynamicFilter(3, 1), ReLu, dReLu);
	addFilter(l2, createDynamicFilter(3, 1));
	addFilter(l2, createDynamicFilter(3, 1));
	addFilter(l2, createDynamicFilter(3, 1));
	addCNNLayer(CNNTab, l2);

	flatten_size = countFlattenSize(CNNTab, DRAWTAB_RESOLUTION, DRAWTAB_RESOLUTION);

	NeuralNetwork* neuralNetworkTab = createNetwork();
	addLayer(neuralNetworkTab, flatten_size, ReLu, dReLu);
	addLayer(neuralNetworkTab, 24, ReLu, dReLu);
	addLayer(neuralNetworkTab, 18, ReLu, dReLu);
	addLayer(neuralNetworkTab, 10, ReLu, dReLu);

	init(neuralNetworkTab);

	drawNetwork(surface, neuralNetworkTab);
	renderSurface(renderer, surface);

	learnTab(tab, DRAWTAB_RESOLUTION, DRAWTAB_RESOLUTION, CNNTab, neuralNetworkTab, 2);

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
					CNNStep(CNNSurface, CNN, neuralNetwork, 1, RGBmode, (float[1]) {-1});
					drawNetwork(surface, neuralNetwork);
					renderSurface(renderer, surface);

					break;
					for (int i = 0; i < 5; i++) {
						step(neuralNetwork, 1);
						drawNetwork(surface, neuralNetwork);
						renderSurface(renderer, surface);
					}

				}
				if (e.key.keysym.sym == SDLK_c)
					clearTab(drawSurface, drawRenderer, tab);
				if (e.key.keysym.sym >= SDLK_0 && e.key.keysym.sym <= SDLK_9) {
					saveTab(tab, e.key.keysym.sym - SDLK_0);
					clearTab(drawSurface, drawRenderer, tab);
				}
				if (e.key.keysym.sym == SDLK_l) {
					FILE* f = fopen("database_chiffres2.txt", "r");
					readLine(f, tab);
					fclose(f);
					displayTab(drawSurface ,tab);
					renderSurface(drawRenderer, drawSurface);
				}
				if (e.key.keysym.sym == SDLK_g) {
					guessTab(tab, CNNTab, neuralNetworkTab);
				}
				break;
			case SDL_MOUSEBUTTONDOWN:
				SDL_GetMouseState(&mousePos.x, &mousePos.y);
				SDL_Rect rect = drawSurface->clip_rect;
				drawing = mousePos.x > rect.x && mousePos.x < rect.x + rect.w && mousePos.y > rect.y && mousePos.y < rect.y + rect.h;
				break;
			case SDL_MOUSEBUTTONUP:
				if (drawing) {
					//for (int j = 0; j < DRAWTAB_RESOLUTION; j++) {
					//	printf("[ ");
					//	for (int i = 0; i < DRAWTAB_RESOLUTION; i++)
					//		printf("%.1f, ", tab[i + j * DRAWTAB_RESOLUTION]);
					//	printf("\b ]\n");
					//}
					//printf("\n");
					drawing = 0;
				}
				break;
			}
		}

		if (drawing)
		{
			SDL_GetMouseState(&mousePos.x, &mousePos.y);
			drawOnTab(drawSurface, mousePos, DRAWTAB_RESOLUTION, tab);
			renderSurface(drawRenderer, drawSurface);
		}

	}

	// Free Memory
	{
		clear(neuralNetwork);
		freeCNNLayer(CNN);
		SDL_FreeSurface(surface);
		SDL_DestroyRenderer(renderer);
		SDL_DestroyWindow(window);

		SDL_FreeSurface(CNNSurface);
		SDL_DestroyRenderer(CNNRenderer);
		SDL_DestroyWindow(CNNview);

		SDL_FreeSurface(drawSurface);
		SDL_DestroyRenderer(drawRenderer);
		SDL_DestroyWindow(drawTab);
		free(tab);
	}

	return EXIT_SUCCESS;
}