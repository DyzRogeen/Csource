#pragma once
#include "utils_SDL.h"
#define PI 3.141592653589793

typedef struct sCercle {
	float m_r;
	float m_w;
	float m_p;
	struct sCercle* m_next;
	int mode;
}cercle;

void addCircle(cercle** circle, float r, float w, float p, int mode);

int affCircle(SDL_Surface* p_affichage, cercle* circle, int p_x, int p_y, int last_Circle, int color);

void removeCircle(cercle** circle);

void freeCircle(cercle** circle);

void straight(cercle** circle, int iter, int max_iter);