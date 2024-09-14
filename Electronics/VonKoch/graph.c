#include "graph.h"


// Dessin
void drawGrid(screen s) {
	int w = s.w->w;
	int h = s.w->h;
	Uint32* pxls = s.w->pixels;

	int zoom = s.zoom;
	int X_start = -s.offsetx % zoom;
	int Y_start = -s.offsety % zoom;
	int slot;

	for (int i = 0; i < w; i += zoom)
		for (int j = 0; j < h; j += zoom) {
			slot = X_start + i + (Y_start + j) * w;
			if (X_start + i >= 0 && X_start + i < w && Y_start + j >= 0 && Y_start + j < h) *(pxls + slot) = GREY;
		}

}

void draw(screen s, point p1, point p2, type t, Uint32 color, int selected) {
	switch (t) {
		case GENERATEUR:
			drawG(s, p1, p2, color, selected);
			break;
		case RESISTANCE:
			drawR(s, p1, p2, color, selected);
			break;
		case BOBINE:
			drawL(s, p1, p2, color, selected);
			break;
		case CONDENSATEUR:
			drawC(s, p1, p2, color, selected);
			break;
		case DIODE:
			drawD(s, p1, p2, color, selected);
			break;
		default:
			drawW(s, p1, p2, color, selected);
			break;
	}
}

void drawW(screen s, point p1, point p2, Uint32 color, int selected) {
	SDL_Surface* win = s.w; int w = win->w, h = win->h; Uint32* pxls = win->pixels;
	int slotx, sloty;

	p1 = getScreenPoint(s, p1);
	p2 = getScreenPoint(s, p2);
	point v = sum(p2, p1, -1), p; float n = norm(v); v = scale(v, 1. / n); point o = orthogonal(v);

	if (selected) {
		color = color >> 8;
		drawBox(s, p1, color);
		drawBox(s, p2, color);
	}

	for (int k = 0; k < n; k++) line3(p1, v, o, k, w, h, pxls, color);
}
void drawG(screen s, point p1, point p2, Uint32 color, int selected) {
	SDL_Surface* win = s.w; int w = win->w, h = win->h; Uint32* pxls = win->pixels;
	int slotx, sloty;

	p1 = getScreenPoint(s, p1);
	p2 = getScreenPoint(s, p2);
	point v = sum(p2, p1, -1), p; float n = norm(v); v = scale(v, 1. / n); point o = orthogonal(v);

	int b_inf = (n - s.zoom) / 2 + 3, b_sup = (n + s.zoom) / 2 - 3;

	if (selected) {
		color = color >> 8;
		drawBox(s, p1, color);
		drawBox(s, p2, color);
	}

	for (int k = 0; k < b_inf; k++) line3(p1, v, o, k, w, h, pxls, color);

	point p_pm = sum(sum(p1, scale(v, b_inf - 10), 1), scale(o, s.zoom/2), 1);
	for (int k = -3; k <= 3; k++) {
		p = sum(p_pm, scale(v, k), 1);
		if (p.x >= 0 && p.x < w && p.y >= 0 && p.y < h)
			*(pxls + (int)p.x + (int)p.y * w) = color;
	}

	p_pm = sum(sum(p1, scale(v, b_sup + 10), 1), scale(o, s.zoom/2), 1);
	for (int k = -3; k <= 3; k++) {
		p = sum(p_pm, scale(v, k), 1);
		if (p.x >= 0 && p.x < w && p.y >= 0 && p.y < h)
			*(pxls + (int)p.x + (int)p.y * w) = color;
		p = sum(p_pm, scale(o, k), 1);
		if (p.x >= 0 && p.x < w && p.y >= 0 && p.y < h)
			*(pxls + (int)p.x + (int)p.y * w) = color;
	}

	p = sum(sum(p1, scale(v, b_inf), 1), scale(o, s.zoom / 2), -1);
	for (int k = 0; k < s.zoom; k++) {
		line3(p, o, v, k, w, h, pxls, color);
	}

	p = sum(sum(p1, scale(v, b_sup), 1), scale(o, s.zoom), -1);
	for (int k = 0; k < s.zoom * 2; k++) {
		line3(p, o, v, k, w, h, pxls, color);
	}

	for (int k = b_sup; k < n; k++) line3(p1, v, o, k, w, h, pxls, color);
}
void drawR(screen s, point p1, point p2, Uint32 color, int selected) {
	SDL_Surface* win = s.w; int w = win->w, h = win->h; Uint32* pxls = win->pixels;
	int slotx, sloty;

	p1 = getScreenPoint(s, p1);
	p2 = getScreenPoint(s, p2);
	point v = sum(p2, p1, -1), p; float n = norm(v); v = scale(v, 1. / n); point o = orthogonal(v);

	int b_inf = n / 2 - (n < 150 ? n / 3 : 50) * s.zoom / 25, b_sup = n / 2 + (n < 150 ? n / 3 : 50) * s.zoom / 25;

	if (selected) {
		color = color >> 8;
		drawBox(s, p1, color);
		drawBox(s, p2, color);
	}

	for (int k = 0; k < b_inf; k++) line3(p1, v, o, k, w, h, pxls, color);

	p = sum(sum(p1, scale(v, b_inf), 1), scale(o, s.zoom / 2), -1);
	for (int k = 0; k < s.zoom; k++) {
		line3(p, o, v, k, w, h, pxls, color);
	}

	for (int k = -1; k < b_sup - b_inf + 2; k++) line3(p, v, o, k, w, h, pxls, color);
	p = sum(p, scale(o, s.zoom), 1);
	for (int k = -1; k < b_sup - b_inf + 2; k++) line3(p, v, o, k, w, h, pxls, color);

	p = sum(sum(p1, scale(v, b_sup), 1), scale(o, s.zoom / 2), -1);
	for (int k = 0; k < s.zoom; k++) {
		line3(p, o, v, k, w, h, pxls, color);
	}

	for (int k = b_sup; k < n; k++) line3(p1, v, o, k, w, h, pxls, color);
}
void drawL(screen s, point p1, point p2, Uint32 color, int selected) {}
void drawC(screen s, point p1, point p2, Uint32 color, int selected) {

	SDL_Surface* win = s.w; int w = win->w, h = win->h; Uint32* pxls = win->pixels;
	int slotx, sloty;

	p1 = getScreenPoint(s, p1);
	p2 = getScreenPoint(s, p2);
	point v = sum(p2, p1, -1), p; float n = norm(v); v = scale(v, 1. / n); point o = orthogonal(v);

	int b_inf = (n - s.zoom) / 2 + 3, b_sup = (n + s.zoom) / 2 - 3;

	if (selected) {
		color = color >> 8;
		drawBox(s, p1, color);
		drawBox(s, p2, color);
	}

	for (int k = 0; k < b_inf; k++) line3(p1, v, o, k, w, h, pxls, color);
	
	p = sum(sum(p1, scale(v, b_inf), 1), scale(o, s.zoom), -1);
	for (int k = 0; k < s.zoom * 2; k++) {
		line3(p, o, v, k, w, h, pxls, color);
	}

	p = sum(sum(p1, scale(v, b_sup), 1), scale(o, s.zoom), -1);
	for (int k = 0; k < s.zoom * 2; k++) {
		line3(p, o, v, k, w, h, pxls, color);
	}

	for (int k = b_sup; k < n; k++) line3(p1, v, o, k, w, h, pxls, color);

}
void drawD(screen s, point p1, point p2, Uint32 color, int selected) {}

void line3(point p, point v, point o, int k, int w, int h, Uint32 * pxls, Uint32 color) {
	int slotx, sloty;

	slotx = p.x + v.x * k;
	sloty = p.y + v.y * k;
	if (slotx >= 0 && slotx < w && sloty >= 0 && sloty < h) *(pxls + slotx + sloty * w) = color;

	slotx = p.x + v.x * k + o.x;
	sloty = p.y + v.y * k + o.y;
	if (slotx >= 0 && slotx < w && sloty >= 0 && sloty < h) *(pxls + slotx + sloty * w) = color;

	slotx = p.x + v.x * k - o.x;
	sloty = p.y + v.y * k - o.y;
	if (slotx >= 0 && slotx < w && sloty >= 0 && sloty < h) *(pxls + slotx + sloty * w) = color;
}

void drawBox(screen s, point p, Uint32 color) {
	SDL_Surface* win = s.w;
	int w = win->w;
	int h = win->h;
	Uint32* pxls = win->pixels;

	for (int i = p.x - 2; i < p.x + 3; i++) {
		if (i < 0 || i >= w) continue;
		for (int j = p.y - 2; j < p.y + 3; j++) {
			if (j < 0 || j >= h) continue;

			*(pxls + i + j * w) = color;
		}
	}


}


// Interactions
void deselectAll(list* L) {
	while (L) {
		L->e->selected = 0;
		L = L->next;
	}
}

elec* select(list* L, point p) {
	point p1, p2, v, o;
	float n, coef;
	while (L) {
		p1 = *L->e->p1;
		p2 = *L->e->p2;
		v = sum(p1, p2, -1);

		// Cas où l'elec est strictement vertical ou horizontal
		/*if (v.x == 0) {
			if (p.y > p1.y && p.y < p2.y || p.y > p2.y && p.y < p1.y) {
				L->e->selected = 1;
				return 1;
			}
			else {
				L = L->next;
				continue;
			}
		}
		if (v.y == 0) {
			if (p.x > p1.x && p.x < p2.x || p.x > p2.x && p.x < p1.x) {
				L->e->selected = 1;
				return 1;
			}
			else {
				L = L->next;
				continue;
			}
		}*/

		n = norm(v); v = scale(v, 1.f / n); o = orthogonal(v);

		coef = projectionCoef(sum(p1, p, -1), v);
		if (coef > 0 && coef < n) {
			coef = projectionCoef(sum(p1, p, -1), o);
			if (coef > -1 && coef < 1) {
				L->e->selected = 1;
				return L->e;
			}
		}

		L = L->next;
	}
	return NULL;
}

point* selectPole(list* L, point p, screen s) {
	elec* e;
	point rp;
	float n;
	while (L) {
		e = L->e;
		rp = getScreenPoint(s, *e->p1);
		n = norm(sum(p, rp, -1));
		if (n < 8.) return e->p1;

		rp = getScreenPoint(s, *e->p2);
		n = norm(sum(p, rp, -1));
		if (n < 8.) return e->p2;

		L = L->next;
	}
	return NULL;
}

// Utils
point setPoint(float x, float y) {
	point p;
	p.x = x;
	p.y = y;
	return p;
}
point sum(point p1, point p2, int diff) {
	p1.x += p2.x * diff;
	p1.y += p2.y * diff;
	return p1;
}
point scale(point p, float f) {
	p.x *= f;
	p.y *= f;
	return p;
}
float norm(point p) {
	return sqrtf(p.x * p.x + p.y * p.y);
}
point unit(point p) {
	return scale(p, 1. / norm(p));
}
point orthogonal(point p) {
	return setPoint(-p.y, p.x);
}
float projectionCoef(point v1, point v2) {
	// de v1 sur v2
	return (v2.x * v1.x + v2.y * v1.y) / (v2.x * v2.x + v2.y * v2.y);
}

// Renvoie le point réèl à afficher à l'écran
point getScreenPoint(screen s, point p) {
	p.x = p.x * s.zoom - s.offsetx;
	p.y = p.y * s.zoom - s.offsety;
	return p;
}
point getSimuPoint(screen s, point p, int P1) {
	if (P1)
		return setPoint(roundf(p.x / s.zoom), roundf(p.y / s.zoom));
	else
		return setPoint(roundf((p.x + s.offsetx) / s.zoom), roundf((p.y + s.offsety) / s.zoom));
}