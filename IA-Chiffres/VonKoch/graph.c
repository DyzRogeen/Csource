#define _CRT_SECURE_NO_WARNINGS 0

#include "graph.h"
#include "utils_SDL.h"
#include <stdlib.h>
#include <math.h>

graph* initGraph(int resolution, int width, int height) {

	graph* g = (graph*)calloc(1, sizeof(graph));

	float* v = (float*)calloc(resolution * resolution, sizeof(float));

	for (int i = 0; i < resolution * resolution; i++)
		v[i] = 0;

	g->width = width;
	g->height = height;
	g->sideSize = resolution;
	g->vector = v;

	return g;

}

void printGraph(graph* g) {

	int sideSize = g->sideSize;
	float* v = g->vector;

	for (int i = 0; i < sideSize; i++) {

		printf("[ ");

		for (int j = 0; j < sideSize; j++)
			printf("%d ", (int)(v[j + i * sideSize] * 10 / 256));

		printf("]\n");

	}

	printf("\n");

}

void affGraph(SDL_Surface* window, graph* g) {

	int sideSize = g->sideSize;
	int width = g->width;
	int height = g->height;
	
	float* v = g->vector;

	int caseSize_x = width / sideSize;
	int caseSize_y = height / sideSize;

	for (int i = 0; i < sideSize; i++)
		for (int j = 0; j < sideSize; j++)
			colorCase(window, i * caseSize_x, j * caseSize_y, caseSize_x, caseSize_y, (Uint32) (v[i + j * sideSize]));

}

void applicatePressure(graph* g, int x, int y) {

	int sideSize = g->sideSize;
	int width = g->width;
	int height = g->height;

	float mouse_x_case = (float) x * sideSize / width;
	float mouse_y_case = (float) y * sideSize / height;

	int x_case = (int)(mouse_x_case - 0.5);
	int y_case = (int)(mouse_y_case - 0.5);

	int d_temp;

	if ((d_temp = dist(x_case + 0.5, y_case + 0.5, mouse_x_case, mouse_y_case)) > g->vector[x_case + y_case * sideSize]) g->vector[x_case + y_case * sideSize] = d_temp;
	if ((d_temp = dist(x_case + 0.5, y_case + 1.5, mouse_x_case, mouse_y_case)) > g->vector[x_case + (y_case + 1) * sideSize])g->vector[x_case + (y_case + 1) * sideSize] = d_temp;
	if ((d_temp = dist(x_case + 1.5, y_case + 0.5, mouse_x_case, mouse_y_case)) > g->vector[x_case + 1 + y_case * sideSize])g->vector[x_case + 1 + y_case * sideSize] = d_temp;
	if ((d_temp = dist(x_case + 1.5, y_case + 1.5, mouse_x_case, mouse_y_case)) > g->vector[x_case + 1 + (y_case + 1) * sideSize])g->vector[x_case + 1 + (y_case + 1) * sideSize] = d_temp;

}


void colorCase(SDL_Surface* window, int x, int y, int width, int height, Uint8 level) {

	Uint32 whiteLevel = level | level << 8 | level << 16;

	for (int i = 0; i < width; i++)
		for (int j = 0; j < height; j++)
			_SDL_SetPixel(window, x + i, y + j, whiteLevel);

}

void resetGraph(SDL_Surface* window, graph* g) {

	int vectorSize = g->sideSize * g->sideSize;
	float* v = g->vector;

	for (int i = 0; i < vectorSize; i++) v[i] = 0;

	affGraph(window, g);
	SDL_UpdateRect(window, 0, 0, 0, 0);
	SDL_Flip(window);

}

int dist(float x1, float y1, float x2, float y2) {
	int d = (int) (255 * (1 - sqrtf((x1 - x2) * (x1 - x2) + (y1 - y2) * (y1 - y2))));
	return d < 0 ? 0 : d;
}

int readV(graph* g, int nLine) {

	printf("Reading data line %d.\n", nLine);

	FILE* f = fopen("database.txt", "r");

	float* v = g->vector;
	int vectorSize = g->sideSize * g->sideSize;

	char c = ' ';
	int i = 0;
	int number, tmpv;

	while (c != EOF && i < nLine) if ((c = fgetc(f)) == '\n') i++;
	if (c == EOF) {
		printf("End Of File !\n");
		fclose(f);
		return -1;
	}

	fscanf(f, "%d", &number);

	for (int i = 0; i < vectorSize; i++) {
		fscanf(f, " %d", &tmpv);
		v[i] = tmpv * 256 / 100;
	}

	g->vector = v;

	printf("number %d.\n", number);
	printGraph(g);

	fclose(f);

	return number;

}

void registerV(graph* g, int number) {

	printf("Saving vector as %d.\n", number);

	FILE* f = fopen("database.txt", "a");

	float* v = g->vector;
	int vectorSize = g->sideSize * g->sideSize;

	fprintf(f, "\n%d", number);

	for (int i = 0; i < vectorSize; i++) fprintf(f, " %d", (int)(v[i] * 100 / 256));

	fclose(f);

}

void freeGraph(graph* g) {
	//free(g->vector);
	free(g);
}