#include "mot.h"
#include "utils_SDL.h"
#include <math.h>

pointf* createPoint(float x, float y) {
	pointf* p = (pointf*)calloc(1, sizeof(pointf));
	p->x = x;
	p->y = y;
	return p;
}

listP* createList(pointf* p) {
	listP* P = (listP*)calloc(1, sizeof(listP));
	P->p = p;
	P->v = setPoint(0, 0);
	P->next = NULL;
	return P;
}

void addPoint(listP** l, pointf* p) {
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
	for (i = 0; points; i++) {
		points = points->next;
	}

	o->nbPoints = i;
	o->color = color;
	if (o->isStatic = isStatic) return o;
	o->mass = mass;
	return o;
}

listO* createListO(obj* o) {
	listO* O = (listO*)calloc(1, sizeof(listO));
	listP* P = o->points;

	int nbP = 0;
	pointf sumP = setPoint(0, 0);
	while (P) {
		sumP = sum(sumP, *(P->p), 1);
		nbP++;
		P = P->next;
	}
	o->pRot = scale(sumP, 1.f / nbP);
	o->vRot = 0.f;
	o->v = setPoint(0.f, 0.f);

	O->o = o;
	O->next = NULL;
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

void verifyOffScreen(SDL_Surface* window, listO* O) {
	if (!O) return;
	verifyOffScreen(window, O->next);

	obj* o = O->o;
	listP* P = o->points;
	pointf p;

	int w = window->w;
	int h = window->h;

	while (P) {
		p = *(P->p);
		if (p.x > 0 && p.x < w && p.y > 0 && p.y < h) return;
		P = P->next;
	}
	printf("pop ! \n");
	freeObj(o);
	if (O->next) O->o = O->next->o;

}

void colorRow(SDL_Surface* window, float x1, float x2, float y, const Uint32 color) {

	int w = window->w;
	if (x1 == x2 || (x1 < 0 || x1 >= w) && (x2 < 0 || x2 >= w)) return;

	int x = (x1 < x2) ? x1 : x2;
	int l = abs(x1 - x2);

	for (int i = 0; i <= l; i += 1) {
		if (x + i >= 0 && x + i < w) *((Uint32*)(window->pixels) + x + i + (int)y * w) = color;
	}

}

void colorTriangle(SDL_Surface* window, pointf p1, pointf p2, pointf p3, const Uint32 color) {

	int max_y = (p1.y > p2.y) ? (p1.y > p3.y ? p1.y : p3.y) : (p2.y > p3.y ? p2.y : p3.y);
	int min_y = (p1.y < p2.y) ? (p1.y < p3.y ? p1.y : p3.y) : (p2.y < p3.y ? p2.y : p3.y);

	pointf v12 = sum(p2, p1, -1);
	pointf v13 = sum(p3, p1, -1);
	pointf v23 = sum(p3, p2, -1);

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
		if ((p1.y < y && y <= p2.y || p1.y > y && y >= p2.y) && (p1.y < y && y <= p3.y || p1.y > y && y >= p3.y) && (v12.y != 0 && v13.y != 0))	colorRow(window, p1.x + (y - p1.y) * v12.x / v12.y, p1.x + (y - p1.y) * v13.x / v13.y, y, color);
		else if ((p1.y <= y && y < p2.y || p1.y >= y && y > p2.y) && (p2.y < y && y <= p3.y || p2.y > y && y >= p3.y) && (v12.y != 0 && v23.y != 0))	colorRow(window, p1.x + (y - p1.y) * v12.x / v12.y, p2.x + (y - p2.y) * v23.x / v23.y, y, color);
		else if (v13.y != 0 && v23.y != 0)																												colorRow(window, p1.x + (y - p1.y) * v13.x / v13.y, p2.x + (y - p2.y) * v23.x / v23.y, y, color);
	}
}

void colorObj(SDL_Surface* window, listP* lP, int nbPoints, Uint32 color) {

	if (nbPoints < 3) return;

	listP* P = lP;
	pointf p = *P->p;

	listP* fpLeft = createList(createPoint(p.x, p.y));

	int nbPointsLeft = nbPoints - (int)(nbPoints / 2);

	for (int i = (int)(nbPoints / 2); i > 0; i--) {

		if (P->next->next) colorTriangle(window, *P->p, *P->next->p, *P->next->next->p, color);
		else {
			colorTriangle(window, *P->p, *P->next->p, *lP->p, color);
			break;
		}

		P = P->next->next;
		p = *P->p;
	}

	colorObj(window, fpLeft, nbPointsLeft, color);
	freeListP(fpLeft);
}

void displayVector(SDL_Surface* window, obj o, pointf p, int scale) {

	pointf v = sum(o.v, rot2v(o, p), 1);
	_SDL_DrawLine(window, p.x, p.y, p.x + v.x * scale, p.y + v.y * scale, 255, 0, 0);

}

void movePoint(obj o, pointf* p) {
	*p = sum(*p, o.v, 1);
	*p = sum(*p, rot2v(o, *p), 1);
}

/*void test(SDL_Surface* window, obj* o, listP* pl) {

	pointf* p = pl->p;
	if (norm(*p) < 0.001f) return;
	pointf v = pl->v;

	printf("TOUCHE ! \n");
	listP* P = o->points;
	pointf* pi;
	pointf vi;
	float coef = 0;

	pointf* pSave = (pointf*)calloc(o->nbPoints - 1, sizeof(pointf));

	int i = 0;

	while (P) {

		if (P != pl) {
			pi = P->p;
			vi = P->v;

			pSave[i] = unit(orthogonal(sum(*pi, *p, -1)));
			float tmp = projectionCoef(vi, pSave[i]);
			_SDL_DrawLine(window, pi->x, pi->y, pi->x + pSave[i].x * tmp * 200, pi->y + pSave[i].y * tmp * 200, 0, 0, 255);

			coef += tmp;

			i++;

		};

		P = P->next;

	}

	P = o->points;
	i = 0;

	while (P) {

		if (P != pl) {
			P->v = scale(pSave[i++], coef);
			P->v.y = -0.01f * v.y;
		}
		P = P->next;

	}

	pl->v = setPoint(0.f, -0.01f * v.y);

	free(pSave);

}

void test2(SDL_Surface* window, obj* o, listP* pc, pointf vc) {

	pointf* p = pc->p;
	if (norm(*p) < 0.001f) return;
	listP* P = o->points;
	pointf v = o->v;
	pointf vect;
	pointf pi;

	float coef = 0;

	while (P) {
		coef += projectionCoef(sum(*P->p, *p, -1), unit(orthogonal(vc)));
		P = P->next;
	}
	P = o->points;
	while (P) {
		//P = scale(unit(orthogonal(sum(*P->p, *p, -1))), coef);
		P = P->next;
	}

}*/

void seekCollision(listO* O) {
	listO* O2 = O;
	obj *o1, *o2;

	while (O) {

		o1 = O->o;

		while (O2) {

			o2 = O2->o;

			if (o1 != o2 && longestRange(o1) + longestRange(o2) >= norm(sum(o1->pRot, o2->pRot, -1))) {
				collide(o1, o2);
			}

			O2 = O2->next;
		}

		O = O->next;
	}


}

// Détection asymétrique, collision d'un sommet de o1 dans o2
void collide(obj* o1, obj* o2) {

	listP* P1 = o1->points;
	listP* P2;
	pointf p1, p2, segc1, segc2, v2;
	//float min1, max1, min2, max2;
	float proj;
	int hasNext = 1;

	while (P1)
	{
		p1 = *(P1->p);
		P2 = o2->points;
		while (P2) {
			p2 = *(P2->p);
			hasNext = (P2->next);

			/*v2 = sum(hasNext ? *(P2->next->p) : *(o2->points->p), p2, -1);
			projSeg(p2, v2, P1, &min1, &max1);
			projSeg(p2, v2, P2, &min2, &max2);
			if (!(
				(min1 < min2 && min2 > max1 || min1 < max2 && max2 > max1) || // Segment 1 intersecte ou contient segment 2 ou
				(min2 < min1 && min1 > max2 && min2 < max1 && max1 > max2))){ // Segment 1 contenu dans segment 2
				break;
			}*/

			proj = projectionCoef(sum(p1, p2, -1), sum(hasNext ? *(P2->next->p) : *(o2->points->p), p2, -1));

			// Si au moins un point est hors d'une projection, il n'y aura pas de collision : on passe ce point.
			if (proj < 0.f && proj > 1.f) break;

			// Si arrivé à la fin, il y a une collision.
			if (!hasNext) {
				getSegContact(p1, (P1->next ? *(P1->next->p) : *(o1->points->p)), o2->points, &segc1.x, &segc1.y, &segc2.x, &segc2.y);
				contact(o1, o2, p1, segc1, segc2);
				break;
			}

			P2 = P2->next;
		}

		P1 = P1->next;
	}

}

void getSegContact(pointf p, pointf pn, listP* P, float *x1, float *y1, float *x2, float *y2) {

	pointf v = sum(pn, p, -1);
	pointf p2, p2n, v2;

	// Coef des vecteurs
	float d, t;

	while (P->next) {
		p2 = *(P->p);
		p2n = *(P->next->p);
		v2 = sum(p2n, p2, -1);

		t = (p.y - p2.y - (p.x - p2.x) * v.y / v.x) / (v2.y - v2.x / (v.x * v.y));
		if (t < 0.f || t > 1.f) {
			P = P->next;
			continue;
		}

		d = (p2.x - p.x + t * v2.x) / v.x;

		printPnt(sum(p, scale(v, d), 1));
		printf(" = ");
		printPnt(sum(p2, scale(v2, t), 1));
		printf("\n");

		if (d < 0.f || d > 1.f) {
			P = P->next;
			continue;
		}

		*x1 = p2.x;
		*y1 = p2.y;
		*x2 = p2n.x;
		*y2 = p2n.y;

		return;
	}


};

void contact(obj* o1, obj* o2, pointf pc1, pointf p1segc2, pointf p2segc2) {
	pointf segc = sum(p1segc2, p2segc2, -1);
	pointf segco1 = sum(o1->pRot, pc1, -1);
	pointf segco2 = sum(o2->pRot, pc1, -1);
	pointf orth = orthogonal(unit(segc));
	pointf vc;
	pointf v1;

	float distRatio = norm(sum(p1segc2, pc1, -1)) / norm(segc);

	// Evol : bounce coef ?
	if (o1->isStatic) {

		// Vecteur réponse au contact
		vc = scale(orth, projectionCoef(sum(o2->v, sum(scale(rot2v(*o2, p1segc2), distRatio), scale(rot2v(*o2, p2segc2), 1 - distRatio), 1), 1), orth));
		// Addition à la vitesse de l'obj
		o2->v = sum(o2->v, scale(unit(segco2), projectionCoef(vc, segco2)), 1);

		o2->vRot += projectionCoef(segco2, orthogonal(vc)) * norm(vc);

	}
	else if (o2->isStatic) {

		//printf("oui\n");

		v1 = sum(o1->v, rot2v(*o1, pc1), 1);

		// Addition à la vitesse de l'obj
		o1->v = sum(o1->v, v1, -1);

		// Vecteur réponse au contact
		vc = scale(orth, projectionCoef(v1, orth) * (v1.x * orth.x + v1.y * orth.y < 0 ? -1 : 1));

		//printf("v1 : (%.1f, %.1f) | vc : (%.1f, %.1f) | o : (%.1f, %.1f)\n", v1.x, v1.y, vc.x, vc.y, o1->v.x, o1->v.y);

		o1->vRot -= projectionCoef(segco1, orthogonal(vc)) * norm(vc) * norm(vc) / 50000;

	}

}

// UTILS

pointf sum(pointf p1, pointf p2, int diff) {
	if (diff != -1) diff = 1;

	p1.x += p2.x * diff;
	p1.y += p2.y * diff;

	return p1;
}

pointf scale(pointf v, float scale) {
	v.x *= scale;
	v.y *= scale;
	return v;
}

pointf setPoint(float x, float y) {
	pointf p;
	p.x = x;
	p.y = y;
	return p;
}

// de v1 sur v2
float projectionCoef(pointf v1, pointf v2) {
	return (v2.x * v1.x + v2.y * v1.y) / (v2.x * v2.x + v2.y * v2.y);
}

pointf orthogonal(pointf v) {
	return setPoint(-v.y, v.x);
}

pointf unit(pointf v) {
	return scale(v, 1 / norm(v));
}

float norm(pointf v) {
	return sqrtf(v.x * v.x + v.y * v.y);
}

// vitesse d'un point issu de la rotation de son objet
pointf rot2v(obj o, pointf p) {
	pointf tan = orthogonal(sum(o.pRot, p, -1));
	return scale(tan, o.vRot);
}

float longestRange(obj* o) {
	float longest = 0.f, tmp;

	listP* P = o->points;
	pointf pRot = o->pRot;

	while (P) {
		if ((tmp = norm(sum(pRot, *P->p, -1))) > longest) longest = tmp;
		P = P->next;
	}

	return longest;
}

void projSeg(pointf p, pointf v, listP* P, float* segMin, float* segMax) {

	float tmp, _min = 0.f, _max = 0.f;
	int minSet = 0, maxSet = 0;
	v = orthogonal(v);

	while (P) {
		tmp = projectionCoef(sum(*P->p, p, -1), v);
		if (tmp < _min || !minSet) {
			_min = tmp;
			minSet = 1;
		}
		if (tmp > _max || !maxSet) {
			_max = tmp;
			maxSet = 1;
		}

		P = P->next;
	}

	*segMin = _min;
	*segMax = _max;

}

// PRINT

void printPnt(pointf p) {
	printf("(%.f , %.f)", p.x, p.y);
}

void printListP(listP* P) {
	printPnt(*(P->p));
	if (!P->next) return;
	printf(" -> ");
	printListP(P->next);
}

void printObj(obj* o) {
	if (o->isStatic)printf("[ STATIC , pRot : ");
	else {
		printf("[v : ");
		printPnt(o->v);
		printf(" , vRot : %1.f , pRot : ", o->vRot);
	}
	printPnt(o->pRot);
	printf(" | P : ");
	printListP(o->points);
	printf("]\n");
}

// FREE

void freeListP(listP* P) {
	if (!P) return;
	freeListP(P->next);
	free(P->p);
	free(P);
}

void freeObj(obj* o) {
	if (!o) return;
	printObj(o);
	freeListP(o->points);
	free(o);
	o = NULL;
}

void freeListO(listO* O) {
	if (!O) return;
	freeListO(O->next);
	freeObj(O->o);
	free(O);
}