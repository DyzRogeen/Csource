#pragma once
#include "utils_SDL.h"
#define PI 3.141592653589793

typedef struct sPoint {
	int m_x;
	int m_y;
}point;

void vonKoch(SDL_Surface* p_affichage, int p_iteration, point p_a, point p_b);

void flocKoch(SDL_Surface* p_affichage, int p_iteration, point p_a, point p_b);

void sierpangle(SDL_Surface* p_affichage, int p_iteration, point p_a, point p_b);

void sierponge(SDL_Surface* p_affichage, int p_iteration, point p_a, point p_b);

void dragon(SDL_Surface* p_affichage, int p_iteration, point p_a, point p_b);