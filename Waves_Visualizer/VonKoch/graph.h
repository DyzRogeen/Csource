#pragma once

#include <stdio.h>
#include "../../Common_Libs/SDL2_image-2.6.3/include/SDL_image.h"
#include "../../Common_Libs/SDL2-2.28.1/include/SDL.h"

#define PI			3.14159265359
#define INV_SQRT2	0.70710678118
#define CUMUL		6.82842712474

#define min(a,b) a < b ? a : b
#define max(a,b) a > b ? a : b

#define C_WHITE 0xFFFFFFFF
#define C_GREY 0xFF808080

typedef struct sPoint {
	int x, y;
}point;

void update(float* tab, int w, int h);

void colorWindow(SDL_Surface* s, float* tab);
Uint32 getColor(float val);