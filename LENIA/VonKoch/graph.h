#pragma once
#include "utils_SDL.h"
#include <math.h>
#include <stdio.h>          
#include <stdlib.h>
#include <time.h>

#define e 2.71828182846

void gaussianDot(SDL_Surface* window, int x, int y, int radius, Uint32 color);

void donutFilter(float* filter, int filterDim, int radius, int width);

void step(SDL_Surface* window, float* growthFunc[3], float* filter[3], int filterDim[3]);

float applyFilter(int x, int y, float* filter, int filterDim, Uint32* pxls, int width, int height, int col);

// UTILS

void fillRandom(SDL_Surface* window);

float norm(int x, int y);

float pxlNorm(Uint32 pixel);

float* mapNorm(Uint32* pixels, int size);

Uint32 toPxl(int val);

int CntActiveFilterCells(float* filter, int filterDim);