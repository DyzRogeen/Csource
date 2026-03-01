#pragma once

#include <math.h>

#include "../../Common_Libs/SDL2_image-2.6.3/include/SDL_image.h"
#include "../../Common_Libs/SDL2-2.28.1/include/SDL.h"

#define max(a, b) a > b ? a : b
#define min(a, b) a < b ? a : b

#define PI	3.14159265359

#define C_WHITE 0xFFFFFFFF
#define C_LGREY 0xFF404040
#define C_RED 0xFFFF0000
#define C_GREEN 0xFF00FF00
#define C_BLUE 0xFF0000FF

typedef struct sPoint {
	int x, y;
}point;

typedef struct sPoint3 {
	float x, y, z;
}point3;

typedef struct sCamera {
	point3 position;
	float lon, lat;
	float plane_dist;
	float UPP; // Units Per Pixel

	// Projection plane
	point3 origin;
	point3 normal, u, v;
}camera;

// Object handling
camera* initCamera(point3 position, float lon, float lat, float UPP);
point3* createPoint(float x, float y, float z);
void freeCamera(camera* cam);

// Graphic methods

void updateProjectionPlane(camera* cam);
point pojectPoint(camera cam, point3 p, int w2, int h2);
void drawGrid(SDL_Surface* s, camera cam);
void drawline(SDL_Surface* s, point p1, point p2, Uint32 color);

void extendVectorToEdges(int* p1_x, int* p1_y, int* p2_x, int* p2_y, int w, int h, int horizon);
void extendAndDrawline(SDL_Surface* s, point p1, point p2, Uint32 color, int h, int w, int horizon);

// Vector methods

point3 sum(point3 p1, point3 p2);
point3 sub(point3 p1, point3 p2);
point3 scale(point3 p, float scale);