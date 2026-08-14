#pragma once

#include <stdio.h>

#include "../../Common_Libs/SDL2_image-2.6.3/include/SDL_image.h"
#include "../../Common_Libs/SDL2-2.28.1/include/SDL.h"

#define max(a, b) a > b ? a : b
#define min(a, b) a < b ? a : b

#define WHITE 0xFFFFFFFF
#define LGREY 0xFFC0C0C0
#define GREY 0xFF808080
#define DGREY 0xFF404040
#define RED 0xFFFF0000
#define GREEN 0xFF00FF00
#define BLUE 0xFF0000FF

#define PI	3.14159265359
#define PI2		6.28318530718
#define PI_2	1.57079632679
#define E	2.718281828459

typedef enum eAstreType {ETOILE, PLANETE, SATTELITE} astreType;

typedef struct sPoint2 {
	float x, y;
}point2;

typedef struct sPoint3 {
	float x, y, z;
}point3;

typedef struct sAstre {
	point3 position, velocity, acceleration;
	float mass, massOrder;
	float radius, radiusOrder;
	float tRotation, lonRotAxis, latRotAxis;
	astreType type;
	SDL_Surface* texture;
	char* name;
}astre;

typedef struct sList {
	struct sList* next;
	struct sList* prec;
	astre* p_astre;
}list;

typedef struct sCam {
	point3 pos, normale, Vx, Vy;
	float d_plan, window_ratio, lat, lon;
}cam;

// Planétaire
astre* createAstre(point3 position, point3 velocity, point3 acceleration, float mass, float massOrder, float radius, float radiusOrder, float tRotation, float lonRotAxis, float latRotAxis, astreType type, char* texture, char name[100]);
list* createList(astre* astre);
void addAstre(list** l, astre* astre);

// Camera and movement
cam* createCam(point3 pos, float lat, float lon, float zoom, float window_ratio);
void moveCam(cam* c, point3 movement);
void rotateCam(cam* c, float d_lat, float d_lon);
int goToAstre(cam* c, astre* a);

// Rendering
point2 projectPoint(cam* c, point3 p, int w, int h);
void renderAstres(SDL_Surface* surface, cam* c, list* astres, int nb_astre, float dT);
void renderAstre(SDL_Surface* surface, cam* c, astre* a);

// Graphics
void mapBackground(SDL_Surface* surface, SDL_Surface* texture, cam* c, float ratio);
void mapPxls(SDL_Surface* destination, SDL_Surface* source);
Uint32 getPxl32(Uint8* pxl);

void drawLine(SDL_Surface* s, point2 p1, point2 p2, Uint32 color);

// Util
point3* createPoint(float x, float y, float z);
point3 sum3(point3 p1, point3 p2, int diff);
point2 sum2(point2 p1, point2 p2, int diff);
point3 scale3(point3 p, float f);
point2 scale2(point2 p, float f);
float norm3(point3 p);
float norm2(point2 p);
point3 unit3(point3 p);
point2 unit2(point2 p);
point3 polaireToCartesien(float lat, float lon);

Uint32 scalePxl(Uint8* pxl, float coef, float val);
float sigmoid(float x);

// Optimistaion Trigo
void init_TUL();
float fast_cos(float x);
float fast_sin(float x);
float fast_acos(float x);
float fast_asin(float x);
float fast_atan(float x);

// FREE
void freeList(list* l);
void freeAstre(astre* a);