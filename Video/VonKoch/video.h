#pragma once

#include "../SDL-1.2.15/include/SDL.h"
#include <math.h>


#define PI	3.14159265359
#define E	2.71828182846

void mapPxls(SDL_Surface* window, SDL_Surface* img);

float* getGaussianDiffFilter(int dim_filter, int verOrHor);

void convolution(SDL_Surface* win, float* filter, int w_filter, int h_filter);

void gaussianNoiseReducer(SDL_Surface* win);

void normIMGs(SDL_Surface* win, Uint32* pxlsH, Uint32* pxlsV);

void blackWhite(SDL_Surface* win);

void detectCorners(SDL_Surface* win);


// UTILS

Uint32* copyPxls(SDL_Surface* win);

float getFilterWeight(float* filter, int w_filter, int h_filter);

void printFilter(float* filter, int w_filter, int h_filter);