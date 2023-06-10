#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include "frac.h"
#include "utils_SDL.h"

/**
*	@brief	Fonction permettant de dessiner une courbe de Koch.
*
*	@param	p_affichage		Fenêtre sur laquelle on souhaite dessiner.
*	@param	p_iteration		Nombre d'itération que l'on souhaite appliquer à la courbe.
*	@param	p_a				Point de départ du segment.
*	@param	p_b				Point d'arrivée du segment.
*/
void vonKoch(SDL_Surface* p_affichage, int p_iteration, point p_a, point p_b)
{

	if (!p_iteration) {

		_SDL_DrawLine(p_affichage, p_a.m_x, p_a.m_y, p_b.m_x, p_b.m_y, 255, 255, 255);
		return;

	}

	point ab, ce, c, d, e;

	ab.m_x = p_a.m_x - p_b.m_x;
	ab.m_y = p_a.m_y - p_b.m_y;

	c.m_x = p_a.m_x - ab.m_x / 3;
	c.m_y = p_a.m_y - ab.m_y / 3;

	d.m_x = p_b.m_x + ab.m_x / 3;
	d.m_y = p_b.m_y + ab.m_y / 3;

	ce.m_x = (ab.m_x * cos(-PI / 3) - ab.m_y * sin(-PI / 3)) / 3;
	ce.m_y = (ab.m_x * sin(-PI / 3) + ab.m_y * cos(-PI / 3)) / 3;

	e.m_x = c.m_x - ce.m_x;
	e.m_y = c.m_y - ce.m_y;

	vonKoch(p_affichage, p_iteration - 1, p_a, c);
	vonKoch(p_affichage, p_iteration - 1, c, e);
	vonKoch(p_affichage, p_iteration - 1, e, d);
	vonKoch(p_affichage, p_iteration - 1, d, p_b);

}

void flocKoch(SDL_Surface* p_affichage, int p_iteration, point p_a, point p_b) {

	point ab, ac, bc, c;

	ab.m_x = p_a.m_x - p_b.m_x;
	ab.m_y = p_a.m_y - p_b.m_y;

	c.m_x = (ab.m_x * cos(-PI / 3) - ab.m_y * sin(-PI / 3)) + p_b.m_x;
	c.m_y = (ab.m_x * sin(-PI / 3) + ab.m_y * cos(-PI / 3)) + p_b.m_y;

	vonKoch(p_affichage, p_iteration, p_a, p_b);
	vonKoch(p_affichage, p_iteration, c, p_a);
	vonKoch(p_affichage, p_iteration, p_b, c);

}

void sierpangle(SDL_Surface* p_affichage, int p_iteration, point p_a, point p_b) {

	point ab, ac, ab1, ab2, c;

	ab.m_x = p_a.m_x - p_b.m_x;
	ab.m_y = p_a.m_y - p_b.m_y;

	ac.m_x = (ab.m_x * cos(PI / 3) - ab.m_y * sin(PI / 3));
	ac.m_y = (ab.m_x * sin(PI / 3) + ab.m_y * cos(PI / 3));

	if (!p_iteration) {

		c.m_x = ac.m_x + p_b.m_x;
		c.m_y = ac.m_y + p_b.m_y;

		_SDL_DrawLine(p_affichage, p_a.m_x, p_a.m_y, p_b.m_x, p_b.m_y, 255, 255, 255);
		_SDL_DrawLine(p_affichage, p_a.m_x, p_a.m_y, c.m_x, c.m_y, 255, 255, 255);
		_SDL_DrawLine(p_affichage, c.m_x, c.m_y, p_b.m_x, p_b.m_y, 255, 255, 255);
		return;

	}

	c.m_x = ac.m_x / 2 + p_b.m_x;
	c.m_y = ac.m_y / 2 + p_b.m_y;

	ab1.m_x = p_a.m_x - ab.m_x / 2;
	ab1.m_y = p_a.m_y - ab.m_y / 2;

	ab2.m_x = c.m_x + ac.m_x / 2;
	ab2.m_y = c.m_y + ac.m_y / 2;

	sierpangle(p_affichage, p_iteration - 1, p_a, ab1);
	sierpangle(p_affichage, p_iteration - 1, ab1, p_b);
	sierpangle(p_affichage, p_iteration - 1, c, ab2);

}

void sierponge(SDL_Surface* p_affichage, int p_iteration, point p_a, point p_b) {

	point ab, c, d;

	ab.m_x = p_b.m_x - p_a.m_x;
	ab.m_y = p_b.m_y - p_a.m_y;

	c.m_x = p_a.m_x + ab.m_y;
	c.m_y = p_a.m_y - ab.m_x;

	d.m_x = c.m_x + ab.m_x;
	d.m_y = c.m_y + ab.m_y;

	if (!p_iteration)
	{
		_SDL_DrawLine(p_affichage, p_a.m_x, p_a.m_y, p_b.m_x, p_b.m_y, 255, 255, 255);
		_SDL_DrawLine(p_affichage, p_a.m_x, p_a.m_y, c.m_x, c.m_y, 255, 255, 255);
		_SDL_DrawLine(p_affichage, c.m_x, c.m_y, d.m_x, d.m_y, 255, 255, 255);
		_SDL_DrawLine(p_affichage, p_b.m_x, p_b.m_y, d.m_x, d.m_y, 255, 255, 255);
		return;
	}

	sierponge(p_affichage, p_iteration - 1, p_a, (point) { ab.m_x / 3 + p_a.m_x, ab.m_y / 3 + p_a.m_y });
	sierponge(p_affichage, p_iteration - 1, (point) { ab.m_x / 3 + p_a.m_x, ab.m_y / 3 + p_a.m_y }, (point) { ab.m_x * 2 / 3 + p_a.m_x, ab.m_y * 2 / 3 + p_a.m_y });
	sierponge(p_affichage, p_iteration - 1, (point) { ab.m_x * 2 / 3 + p_a.m_x, ab.m_y * 2 / 3 + p_a.m_y }, p_b);
	sierponge(p_affichage, p_iteration - 1, (point) { ab.m_y / 3 + p_a.m_x, -ab.m_x / 3 + p_a.m_y }, (point) { ab.m_y / 3 + ab.m_x / 3 + p_a.m_x, -ab.m_x / 3 + ab.m_y / 3 + p_a.m_y });
	sierponge(p_affichage, p_iteration - 1, (point) { ab.m_y * 2 / 3 + p_a.m_x, -ab.m_x * 2 / 3 + p_a.m_y }, (point) { ab.m_y * 2 / 3 + ab.m_x / 3 + p_a.m_x, -ab.m_x * 2 / 3 + ab.m_y / 3 + p_a.m_y });
	sierponge(p_affichage, p_iteration - 1, (point) { ab.m_y * 2 / 3 + ab.m_x / 3 + p_a.m_x, -ab.m_x * 2 / 3 + ab.m_y / 3 + p_a.m_y }, (point) { ab.m_y * 2 / 3 + ab.m_x * 2 / 3 + p_a.m_x, -ab.m_x * 2 / 3 + ab.m_y * 2 / 3 + p_a.m_y });
	sierponge(p_affichage, p_iteration - 1, (point) { ab.m_y * 2 / 3 + ab.m_x * 2 / 3 + p_a.m_x, -ab.m_x * 2 / 3 + ab.m_y * 2 / 3 + p_a.m_y }, (point) { ab.m_y * 2 / 3 + ab.m_x + p_a.m_x, -ab.m_x * 2 / 3 + ab.m_y + p_a.m_y });
	sierponge(p_affichage, p_iteration - 1, (point) { ab.m_y / 3 + ab.m_x * 2 / 3 + p_a.m_x, -ab.m_x / 3 + ab.m_y * 2 / 3 + p_a.m_y }, (point) { ab.m_y / 3 + ab.m_x + p_a.m_x, -ab.m_x / 3 + ab.m_y + p_a.m_y });

}

void dragon(SDL_Surface* p_affichage, int p_iteration, point p_a, point p_b);