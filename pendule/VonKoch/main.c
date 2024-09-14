#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include "utils_SDL.h"

#define PI	3.14159265359
#define G -9.81

typedef struct sPoint{
	float x, y;
}point;
typedef struct sDot {
	float theta, m, T;
	point pos, v;
	struct sDot* next, *prec;
}dot;

point setPoint(float x, float y) {
	point p;
	p.x = x;
	p.y = y;
	return p;
}

dot* createDot(point pos) {
	dot* d = (dot*)calloc(1, sizeof(dot));
	d->pos = pos;
	d->m = 1;
	d->next = NULL;
	d->prec = NULL;
	return d;
}

void addDot(dot** D, dot* d) {
	if (!D) return;
	if (*D) {
		(*D)->prec = d;
		d->next = *D;
	}
	*D = d;
}

void freeDot(dot* D) {
	if (!D) return;
	freeDot(D->next);
	free(D);
}

point sum(point p1, point p2, int diff) {
	diff = (diff == -1) ? -1 : 1;
	p1.x += p2.x * diff;
	p1.y += p2.y * diff;
	return p1;
}

void add(point *p1, point p2, int diff) {
	diff = (diff == -1) ? -1 : 1;
	p1->x += p2.x * diff;
	p1->y += p2.y * diff;
}

point scale(point p, float k) {
	p.x *= k;
	p.y *= k;
	return p;
}

float norm(point p) {
	return sqrtf(p.x * p.x + p.y * p.y);
}

point unit(point p) {
	return scale(p, 1.f / norm(p));
}

point orthogonal(point v) {
	return setPoint(-v.y, v.x);
}
// de v1 sur v2
float projectionCoef(point v1, point v2) {
	return (v2.x * v1.x + v2.y * v1.y) / (v2.x * v2.x + v2.y * v2.y);
}

void drawLine(SDL_Surface* window, point p1, point p2, Uint32 col, int step) {

	int w = window->w;
	int h = window->h;
	Uint32* pxls = window->pixels;

	point v = sum(p1, p2, -1);
	point u = unit(v);
	float n = norm(v);

	point s;
	for (int k = 0; k < n; k += step) {
		s = sum(p2, scale(u, k), 1);
		if (s.x < 0 || s.x >= w || s.y < 0 || s.y >= h) continue;

		*(pxls + (int)s.x + (int)(s.y) * w) = col;
	}
}

void update(SDL_Surface* window, dot* D, int ticks) {
	int w = window->w;
	int h = window->h;
	Uint32* pxls = window->pixels;

	point p, pn, ray, ray2, F;
	float thetacc, theta, nray, coef = 0.0001;
	dot* tmp;
	while (D->next) {
		p = D->pos;
		pn = D->next->pos;

		// A opti si nécéssaire // Jusqu'à pendule double seulement, voir partie commentée pour les autres
		ray = sum(p, pn, -1);
		nray = norm(ray);
		theta = D->theta = (ray.y > 0) ? asin(ray.x / nray) : (ray.x < 0 ? -PI : PI) - asin(ray.x / nray); // Angle par rapport à la verticale.
		tmp = D->prec;
		if (tmp) {
			thetacc = coef * (-G * sin(theta) / nray - tmp->T * sin(theta + tmp->theta) / D->m); // Accélération angulaire (formule pendule double).
		} else thetacc = - coef * G * sin(theta) / nray; // Formule pendule simple.

		D->T = D->m * (norm(D->v) * norm(D->v) * nray + G * cos(theta)) * 0.001; // Tension du câble.

		point acctan = setPoint(-thetacc * nray * cos(theta), thetacc * nray * sin(theta)); // Accélération linéaire tangeante.

		/*thetacc = - coef * G * sin(theta) / nray; // Accélération angulaire (formule pendule simple).
				
		point acc = setPoint(-thetacc * cos(theta), thetacc * sin(theta)); // Accélération linéaire du point.

		// Perturbations de l'accélération liées au pendule du dessous (force centrifuge + poids).
		tmp = D->prec;
		if (tmp) {
			ray2 = sum(tmp->pos, p, -1);
			F = scale(ray2, (powf(norm(tmp->v) / norm(ray2), 2.f) + projectionCoef(setPoint(0, 0.001), ray2)) * nray);
			drawLine(window, p, sum(p, scale(F, 0.1), 1), 200 << 8, 1);
			drawLine(window, p, sum(p, scale(unit(acc), 0.1 * projectionCoef(F, unit(acc))), 1), 255 << 8, 1);
			//acc = setPoint(scale(acc, coef).x + acc.x, scale(acc, coef).y + acc.y);
			acc = scale(acc, 1 + projectionCoef(F, acc) * coef * 0.1);
		}
		*/
		// Redressement pour éviter un allongement de rayon.

		add(&D->v, scale(ray, -powf(norm(D->v) / nray, 2.f)), 1); // Accélération linéaire centripète.
		add(&D->v, acctan, 1);
	
		tmp = D;
		while (tmp) {
			add(&D->pos, tmp->v, 1);
			tmp = tmp->next;
		}
		
		drawLine(window, p, sum(p, scale(D->v, 100), 1), 255 << 16, 1);
		drawLine(window, p, sum(p, scale(acctan, 10000), 1), 255, 1);
		drawLine(window, p, D->next->pos, 255 << 16 | 255 << 8 | 255, 5);

		//printf("[%.1f ; %.1f] %.1f %.1f\n", D->v.x, D->v.y, norm(D->v), 180 * theta / PI);
		//printf("%f\n", norm(ray));

		D = D->next;
	}

}

int main(int argc, char **argv)
{
	int ticks = 0;
	dot* D = NULL;
	addDot(&D, createDot(setPoint(320, 100)));

	SDL_Surface * window;
	int quit = 0;
	SDL_Event e;
	int mPosx, mPosy;

	if (SDL_Init(SDL_INIT_VIDEO) < 0) {
		fprintf(stderr, "ERREUR - SDL_Init\n>>> %s\n", SDL_GetError());
		exit(EXIT_FAILURE);
	}

	atexit(SDL_Quit);
	window = SDL_SetVideoMode(640, 480, 32, SDL_HWSURFACE | SDL_DOUBLEBUF);

	if (window == NULL) {
		fprintf(stderr, "ERREUR - impossible de passer en : %dx%dx%d\n>>> %s\n", 640, 480, 32, SDL_GetError());
		exit(EXIT_FAILURE);
	}

	SDL_WM_SetCaption("VonKoch", NULL);

	while (!quit)
	{
		while (SDL_PollEvent(&e))
		{
			switch (e.type)
			{
			case SDL_QUIT:
				quit = 1;
				break;
			case SDL_KEYDOWN:
				if (e.key.keysym.sym == SDLK_ESCAPE)
					quit = 1;
				break;
			case SDL_MOUSEBUTTONDOWN:
				SDL_GetMouseState(&mPosx, &mPosy);
				addDot(&D, createDot(setPoint(mPosx, mPosy)));
				break;
			}
		}

		if (1)
		{
			SDL_FillRect(window, &(window->clip_rect), SDL_MapRGB(window->format, 0, 0, 0));
			update(window, D, ticks);
			SDL_UpdateRect(window, 0, 0, 0, 0);
			SDL_Flip(window);
			if (ticks < 10) ticks++;
			else ticks = 0;
		}

	}

	freeDot(D);

	return EXIT_SUCCESS;
}