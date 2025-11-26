#pragma once

#include <math.h>
#include <omp.h>
//#include "../../Common_Libs/SDL-1.2.15/include/SDL.h"
#include "../../Common_Libs/SDL2_image-2.6.3/include/SDL_image.h"
#include "../../Common_Libs/SDL2-2.28.1/include/SDL.h"
#include "mandelbrot.h"

#define C_BLACK 0x0
#define C_RED 0xFFFF0000
#define C_WHITE 0xFFFFFFFF

void drawJulia(SDL_Surface* s, pointf offset, double UPP, pointf c, float power, int nb_iter);
void drawMandelbrot(SDL_Surface* s, pointf offset, double UPP, float power, int nb_iter);
void drawAxis(SDL_Surface* s, pointf offset, double UPP);
void drawC(SDL_Surface* s, pointf offset, double UPP, pointf c);
void drawCursor(SDL_Surface* s);

void invertColor(Uint32* color);
Uint32 getColor(int val, int val_max);