#include "graph.h"

void gaussianDot(SDL_Surface* window, int x, int y, int radius, Uint32 color) {

	float coef;
	Uint8* rgb = (Uint8*)&color;
	Uint32 nColor;

	Uint32* pixels = window->pixels;
	Uint8* pixel;
	int width = window->w;
	int height = window->h;

	for (int i = -radius; i < radius; i++) {

		if (x + i < 0 || x + i >= width) continue;

		for (int j = -radius; j < radius; j++) {

			if (y + j < 0 || y + j >= height) continue;

			pixel = (Uint8*)(pixels + x + i + (y + j) * width);

			coef = 1 - norm(i, j) / radius;
			coef = max(0, coef);

			pixel[0] = min(255, pixel[0] + rgb[0] * coef);
			pixel[1] = min(255, pixel[1] + rgb[1] * coef);
			pixel[2] = min(255, pixel[2] + rgb[2] * coef);

		}
	}

}

void donutFilter(float* filter, int filterDim, int radius, int width) {

	width = (width + 1) / 2;
	float dist = radius - width;

	for (int i = 0; i < filterDim; i++) {
		//printf("[ ");
		for (int j = 0; j < filterDim; j++) {
			filter[i + j * filterDim] = min(1., max(filter[i + j * filterDim], 1 - fabs(norm(i - (filterDim - 1) / 2, j - (filterDim - 1) / 2) - dist) / width));
			//printf("%.1f ", filter[i + j * radius * 2]);
		}
		//printf("]\n");
	}
}

void step(SDL_Surface* window, float* growthFunc[3], float* filter[3], int filterDim[3]) {

	Uint32* pixels = window->pixels;
	Uint8* pixel;
	int width = window->w;
	int height = window->h;

	int activeCnt[3] = { CntActiveFilterCells(filter[0], filterDim[0]), CntActiveFilterCells(filter[1], filterDim[1]), CntActiveFilterCells(filter[2], filterDim[2]) };

	float growthRate[3];

	for (int x = 0; x < width; x++)
		for (int y = 0; y < height; y++) {

			pixel = (Uint8*)(pixels + x + y * width);

			growthRate[0] = growthFunc[0][(int)(applyFilter(x, y, filter[0], filterDim[0], pixels, width, height, 0) * 100 / activeCnt[0])];
			growthRate[1] = growthFunc[1][(int)(applyFilter(x, y, filter[1], filterDim[1], pixels, width, height, 1) * 100 / activeCnt[1])];
			growthRate[2] = growthFunc[2][(int)(applyFilter(x, y, filter[2], filterDim[2], pixels, width, height, 2) * 100 / activeCnt[2])];

			pixel[0] = max(0, min(255, pixel[0] + 255 * (70 * growthRate[0] + 10 * growthRate[1] + 20 * growthRate[2]) / 100));
			pixel[1] = max(0, min(255, pixel[1] + 255 * (15 * growthRate[0] + 65 * growthRate[1] + 20 * growthRate[2]) / 100));
			pixel[2] = max(0, min(255, pixel[2] + 255 * (03 * growthRate[0] + 07 * growthRate[1] + 90 * growthRate[2]) / 100));

			*(pixels + x + y * width) = (pixel[0] << 16 | pixel[1] << 8 | pixel[2]);

		}

	printf("Done.\n");

}

float applyFilter(int x, int y, float* filter, int filterDim, Uint32* pxls, int width, int height, int col) {

	float sum = 0;
	int new_x, new_y;

	for (int i = -filterDim / 2; i < filterDim / 2; i++) {

		//if (x + i < 0 || x + i >= width) continue;
		new_x = x < -i ? x + i + width : (x + i) % width;

		for (int j = -filterDim / 2; j < filterDim / 2; j++) {

			//if (y + j < 0 || y + j >= height) continue;
			new_y = y < -j ? y + j + height : (y + j) % height;

			sum += ((Uint8*)(pxls + new_x + new_y * width))[col] * filter[i + filterDim / 2 + (j + filterDim / 2) * filterDim] / 255.0;

		}
	}

	return sum;

}

// UTILS

void fillRandom(SDL_Surface* window) {

	srand(time(NULL));

	Uint32* pixels = window->pixels;
	int width = window->w;
	int height = window->h;

	for (int i = 0; i < width; i++)
		for (int j = 0; j < height; j++)
			*(pixels + i + j * width) = toPxl(rand() % 256);

}

float norm(int x, int y) {
	return sqrt(x * x + y * y);
}

float pxlNorm(Uint32 pixel) {
	Uint8* rgb = (Uint8*)&pixel;
	return (rgb[0] + rgb[1] + rgb[2]) / 765.;
}

float* mapNorm(Uint32* pixels, int size) {

	float* norms = (float*)calloc(size, sizeof(float));

	for (int i = 0; i < size; i++)
		norms[i] = pxlNorm(pixels[i]);

	return norms;

}

Uint32 toPxl(int val) {
	return (Uint32)(val << 16 | val << 8 | val);
}

int CntActiveFilterCells(float* filter, int filterDim) {

	int activeCnt = 0;
	for (int i = 0; i < filterDim; i++)
		for (int j = 0; j < filterDim; j++)
			activeCnt += (0.1 < filter[i + j * filterDim]);

	return activeCnt;

}