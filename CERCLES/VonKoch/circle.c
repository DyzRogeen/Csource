#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include "circle.h"
#include "utils_SDL.h"

void addCircle(cercle** circle, float r, float w, float p, int mode) {

	if (!circle) return;

	if (!*circle) {
		cercle* new = (cercle*)calloc(1, sizeof(cercle));
		new->m_next = NULL;
		new->m_r = r;
		new->m_w = w * 2 * PI;
		new->m_p = p * PI;
		new->mode = mode;
		*circle = new;
		return;
	}

	addCircle(&(*circle)->m_next, r, w, p, mode);

}

int affCircle(SDL_Surface* p_affichage, cercle* circle, int p_x, int p_y, int last_Circle, int color) {

	if (!circle) return 1;

	Uint32 coul;
	if (color)
		coul = SDL_MapRGB(p_affichage->format, 128 + 127 * cos(SDL_GetTicks() / 100), 128 + 127 * cos(SDL_GetTicks() / 100 + PI * 2 / 3), 128 + 127 * cos(SDL_GetTicks() / 10 + PI * 4 / 3));
	else
		coul = SDL_MapRGB(p_affichage->format, 255, 255, 255);

	float r = circle->m_r;
	float w = circle->m_w;
	float p = circle->m_p;
	int m = circle->mode;
	int px = p_x + r * cos(w * SDL_GetTicks() / 1000 + p);
	int py = p_y + r * sin(w * SDL_GetTicks() / 1000 + p);

	if (last_Circle == 0)
		_SDL_SetPixel(p_affichage, px * (m != 2) + p_x * (m == 2), py * (m != 1) + p_y * (m == 1), coul);

	if (affCircle(p_affichage, circle->m_next, px*(m!=2) + p_x*(m==2), py*(m!=1) + p_y*(m==1), last_Circle, color) == 1 && last_Circle == 1)
		_SDL_SetPixel(p_affichage, px * (m != 2) + p_x * (m == 2), py * (m != 1) + p_y * (m == 1), coul);

	return 0;
}

void removeCircle(cercle** circle) {

	if (!circle || !*circle) {
		printf("REMOVECIRCLE : ERROR : Last circle could not be removed.\n");
		return;
	}

	if (!(*circle)->m_next) {

		(*circle)->m_r = .0;
		(*circle)->m_w = .0;
		(*circle)->m_p = .0;

		free((*circle)->m_next);
		(*circle)->m_next = NULL;
		free(*circle);

		printf("REMOVECIRCLE : Last circle successfully removed.\n");
		return;

	}

	removeCircle(&(*circle)->m_next);

	if ((*circle)->m_next && (*circle)->m_next->m_next)
		(*circle)->m_next = NULL;

}

void freeCircle(cercle** circle) {

	if (!circle || !*circle) return;

	freeCircle(&(*circle)->m_next);

	free(*circle);

}

void straight(cercle** circle, int iter, int max_iter) {

	if (iter == max_iter) return;

	float r = 50 * (1-pow(-1, iter))/iter;

	if (r!=.0)
		addCircle(circle, r, 0.5*iter, 0, 2);

	straight(circle, iter + 1, max_iter);

}