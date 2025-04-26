#pragma once

#define _CRT_SECURE_NO_WARNINGS 1
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include "utils_SDL.h"
#include "elec.h"

#define WHITE 255 << 16 | 255 << 8 | 255
#define CYAN 255 << 8 | 255
#define LGREY 152 << 16 | 152 << 8 | 152
#define GREY 128 << 16 | 128 << 8 | 128
#define DGREY 64 << 16 | 64 << 8 | 64
#define DDGREY 16 << 16 | 16 << 8 | 16
#define LRED 255 << 16
#define RED 128 << 16
#define LBLUE 255
#define BLUE 128
#define GREEN 128 << 8
#define YELLOW 200 << 16 | 200 << 8

typedef enum elemType {TEXT}eType;

typedef struct sGraph {
	float UpixRatio, IpixRatio, secPixRatio;
	float *Utab, *Itab;
	int tmpLast, index, tabLength, overflow;
	int toDraw;
}graph;

typedef struct sScreen {
	SDL_Surface* w;
	graph* g;
	int offsetx, offsety, zoom;
}screen;

typedef struct sIconCase {
	type t;
	int size, z, pressed;
	struct sIconCase* next;
}icon;

typedef struct sWindowElem {
	eType type;
	char* label;
	struct sWindowElem* next;
}welem;

welem* createWElem(eType type, char* label);
void freeWElem(welem* w);

graph* createGraph(float UpixRatio, float IpixRatio, float secPixRatio);
void freeGraph(graph* g);

icon* createIcon(int size, int z, int pressed, type t);
void addIcon(icon** I, icon* i);
void freeIcons(icon* I);
icon* initIcons();

void drawGrid(screen s);

void draw(screen s, point p1, point p2, type t, Uint32 color, int selected, int isIcon);

void drawW(screen s, point p1, point p2, Uint32 color, int selected, int isIcon);
void drawG(screen s, point p1, point p2, Uint32 color, int selected, int isIcon);
void drawAD(screen s, point p1, point p2, Uint32 color, int selected, int isIcon);
void drawR(screen s, point p1, point p2, Uint32 color, int selected, int isIcon);
void drawL(screen s, point p1, point p2, Uint32 color, int selected, int isIcon);
void drawC(screen s, point p1, point p2, Uint32 color, int selected, int isIcon);
void drawD(screen s, point p1, point p2, Uint32 color, int selected, int isIcon);
void drawVCC(screen s, point p1, point p2, Uint32 color, int selected, int isIcon);
void drawGND(screen s, point p1, point p2, Uint32 color, int selected, int isIcon);

void line(point p, point v, int k, int w, int h, Uint32* pxls, Uint32 color);
void line3(point p, point v, point o, int k, int w, int h, Uint32* pxls, Uint32 color);
void drawLine(point p1, point p2, int w, int h, Uint32* pxls, Uint32 color);
void drawLine3(point p1, point p2, int w, int h, Uint32* pxls, Uint32 color);

void drawBox(screen s, point p, Uint32 color);
void drawSelectedArea(SDL_Surface* s, point p1, point p2);

void drawGraph(screen s, elec* e, float Ttmp);
void drawParamBar(screen s, elec* e);
void drawGUI(screen s, icon* I);
void drawGUIBox(screen s, icon* i);

void selectZone(screen s, list* L, point p1, point p2);
void deselectAll(list* L);
elec* select(list* L, point p);
point* selectPole(list* L, point p, screen s);
type selectType(icon* I, point p);
void unpressAll(icon* I);

void drawCurrent(screen s, list* L, int T);

point setPoint(float x, float y);
point sum(point p1, point p2, int diff);
point scale(point p, float f);
float norm(point p);
point unit(point p);
point orthogonal(point p);
float projectionCoef(point v1, point v2);
Uint32 getColor(float V);
Uint32 addColor(Uint32 color1, Uint32 color2);
Uint32 scaleColor(Uint32 color, float n);

point getScreenPoint(screen s, point p);
point getSimuPoint(screen s, point p, int P1);