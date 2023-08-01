#include "mot.h"
#include "utils_SDL.h"

point sum(point p1, point p2, int diff) {
	if (diff != -1) diff = 1;

	p1.x += p2.x * diff;
	p1.y += p2.y * diff;

	return p1;
}

point setPoint(int x, int y) {
	point p;
	p.x = x;
	p.y = y;
	return p;
}

point* createPoint(int x, int y) {
	point* p = (point*)calloc(1, sizeof(point));
	p->x = x;
	p->y = y;
	return p;
}

listP* createList(point* p) {
	listP* P = (listP*)calloc(1, sizeof(listP));
	P->p = p;
	return P;
}

void freeListP(listP* P) {
	if (!P) return;
	freeListP(P->next);
	free(P->p);
	free(P);
}

void addPoint(listP** l, point* p) {
	if (!l) return;
	if (!*l) {
		*l = createList(p);
		return;
	}
	listP* new_list = createList(p);
	new_list->next = *l;
	*l = new_list;
}

obj* createObj(listP* points, Uint32 color, float mass, int isStatic) {
	obj* o = (obj*)calloc(1, sizeof(obj));
	o->points = points;

	int i;
	for (i = 0; points; i++) points = points->next;

	o->nbPoints = i;
	o->color = color;
	if (o->isStatic = isStatic) return o;
	o->mass = mass;
	o->velocity.x = 0;
	o->velocity.y = 0;
	return o;
}

listO* createListO(obj* o) {
	listO* O = (listO*)calloc(1, sizeof(listO));
	O->o = o;
	return O;
}

void addObj(listO** O, obj* o) {
	if (!O) return;
	if (!*O) {
		*O = createListO(o);
		return;
	}
	listO* new_list = createListO(o);
	new_list->next = *O;
	*O = new_list;
}

void colorRow(SDL_Surface* window, int x1, int x2, int y, const Uint32 color) {

	int w = window->w;
	if (x1 == x2 || (x1 < 0 || x1 >= w) && (x2 < 0 || x2 >= w)) return;

	int inc = (x1 < x2) ? 1 : -1;
	int x = (x1 < x2) ? x1 : x2;
	int l = abs(x1 - x2);

	for (int i = 0; i <= l; i += 1) {
		if (x + i >= 0 && x + i < w) *((Uint32*)(window->pixels) + x + i + y * w) = color;
	}

}

void colorTriangle(SDL_Surface* window, point p, point p2, point p3, const Uint32 color) {

	int max_y = (p.y > p2.y) ? (p.y > p3.y ? p.y : p3.y) : (p2.y > p3.y ? p2.y : p3.y);
	int min_y = (p.y < p2.y) ? (p.y < p3.y ? p.y : p3.y) : (p2.y < p3.y ? p2.y : p3.y);

	point v12 = sum(p2, p, -1);
	point v13 = sum(p3, p, -1);
	point v23 = sum(p3, p2, -1);

	/*
	*         *                      Filling the triangle from top to bottom
	*        *===*					 by finding the X of 2 of the edges in its
	*       *=======*                intersection with the Y value.
	*      *===========*             v.x/v.y being the slope value from the
	*---->*===============*<-------- Y axis perspective.
	*    *                   *
	*   *                       *
	*  *'''''''''''''''''''''''''''*
	*/

	for (int y = min_y + (v12.y != 0 && v13.y != 0 && v23.y != 0); y < max_y; y += 1) {
		if (y < 0 || y >= window->h) continue;
		if ((p.y < y && y <= p2.y || p.y > y && y >= p2.y) && (p.y < y && y <= p3.y || p.y > y && y >= p3.y) && (v12.y != 0 && v13.y != 0))			colorRow(window, p.x + (y - p.y) * v12.x / v12.y, p.x + (y - p.y) * v13.x / v13.y, y, color);
		else if ((p.y <= y && y < p2.y || p.y >= y && y > p2.y) && (p2.y < y && y <= p3.y || p2.y > y && y >= p3.y) && (v12.y != 0 && v23.y != 0))	colorRow(window, p.x + (y - p.y) * v12.x / v12.y, p2.x + (y - p2.y) * v23.x / v23.y, y, color);
		else if (v13.y != 0 && v23.y != 0)																											colorRow(window, p.x + (y - p.y) * v13.x / v13.y, p2.x + (y - p2.y) * v23.x / v23.y, y, color);
	}
}

void colorObj(SDL_Surface* window, listP* lP, int nbPoints, Uint32 color) {

	listP* P = lP;
	point p = *P->p;

	listP* pSaved = createList(createPoint(p.x, p.y));
	listP* fpLeft;

	int nbPointsLeft = nbPoints - (int)(nbPoints / 2);

	fpLeft = pSaved;

	for (int i = (int)(nbPoints / 2); i > 0; i--) {

		if (P->next->next) colorTriangle(window, *P->p, *P->next->p, *P->next->next->p, color);
		else {
			colorTriangle(window, *P->p, *P->next->p, *lP->p, color);
			break;
		}

		P = P->next->next;
		p = *P->p;

		pSaved->next = createList(createPoint(p.x, p.y));
		pSaved = pSaved->next;
	}

	if (nbPointsLeft > 2) colorObj(window, fpLeft, nbPointsLeft, color);
	freeListP(pSaved);
}

void moveObj(obj* o) {
	pointf v = o->velocity;
	listP* P = o->points;
	point* p;

	for (int i = 0; P; i++) {
		p = P->p;
		p->x += v.x;
		p->y += v.y;
		P = P->next;
	}
}