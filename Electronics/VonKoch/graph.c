#include "graph.h"

// POPUPS POUR RENSEIGNER VALEURS
welem* createWElem(eType type, char* label) {
	welem* w = (welem*)calloc(1, sizeof(welem));
	w->type = type;
	strcpy(w->label, label);
	w->next = NULL;
	return w;
}
void freeWElem(welem* w) {
	if (w->next) freeWElem(w->next);
	free(w);
}

graph* createGraph(float UpixRatio, float IpixRatio, float secPixRatio) {
	graph* g = (graph*)calloc(1, sizeof(graph));

	g->tmpLast = -1;
	g->overflow = 0;
	g->index = 0;
	g->toDraw = 1;

	g->UpixRatio = UpixRatio; // pxl/V
	g->IpixRatio = IpixRatio * 1000; // pxl/mA
	g->secPixRatio = secPixRatio; // pxl/s
	g->tabLength = 6480 / secPixRatio; // Largeur graph : 648 et on veut le nombre de ms à faire rentrer dedans
	g->Utab = (float*)calloc(g->tabLength, sizeof(float));
	g->Itab = (float*)calloc(g->tabLength, sizeof(float));

	return g;
}
void freeGraph(graph* g) {
	free(g->Utab);
	free(g->Itab);
	free(g);
}

icon* createIcon(int size, int z, int pressed, type t) {
	icon* i = (icon*)calloc(1, sizeof(icon));
	i->next = NULL;
	i->pressed = pressed;
	i->size = size;
	i->z = z;
	i->t = t;
	return i;
}
void addIcon(icon** I, icon* i) {
	if (!I) return;
	if (!*I) {
		*I = i;
		return;
	}
	i->next = *I;
	*I = i;
}
void freeIcons(icon* I) {
	if (!I) return;
	freeIcons(I->next);
	free(I);
}
icon* initIcons() {
	icon* I = NULL;
	for (type t = 0; t <= WIRE; t++)
		addIcon(&I, createIcon(18, 45 * t + 25, t==WIRE, t));
	return I;
}

// Dessin
void drawGrid(screen s) {
	int w = s.w->w, rectW = s.w->clip_rect.w;
	int h = s.w->h, rectH = s.w->clip_rect.h;
	Uint32* pxls = s.w->pixels;

	int zoom = s.zoom;
	int X_start = (- 2 - s.offsetx) % zoom;
	int Y_start = -s.offsety % zoom;
	int slot;

	for (int i = 50; i < rectW; i += zoom)
		for (int j = 0; j < rectH; j += zoom) {
			slot = X_start + i + (Y_start + j) * w;
			if (X_start + i >= 0 && X_start + i < w && Y_start + j >= 0 && Y_start + j < h) *(pxls + slot) = GREY;
		}

}

void draw(screen s, point p1, point p2, type t, Uint32 color, int selected, int isIcon) {
	switch (t) {
		case VCC:
			drawVCC(s, p1, p2, color, selected, isIcon);
			break;
		case GND:
			drawGND(s, p1, p2, color, selected, isIcon);
			break;
		case GENERATEUR:
			drawG(s, p1, p2, color, selected, isIcon);
			break;
		case RESISTANCE:
			drawR(s, p1, p2, color, selected, isIcon);
			break;
		case BOBINE:
			drawL(s, p1, p2, color, selected, isIcon);
			break;
		case CONDENSATEUR:
			drawC(s, p1, p2, color, selected, isIcon);
			break;
		case DIODE:
			drawD(s, p1, p2, color, selected, isIcon);
			break;
		case WIRE:
			drawW(s, p1, p2, color, selected, isIcon);
			break;
	}
}

void drawW(screen s, point p1, point p2, Uint32 color, int selected, int isIcon) {
	SDL_Surface* win = s.w; int w = win->w, h = win->h; Uint32* pxls = win->pixels;
	int slotx, sloty;

	if (!isIcon) {
		p1 = getScreenPoint(s, p1);
		p2 = getScreenPoint(s, p2);
	}
	point v = sum(p2, p1, -1), p; float n = norm(v); v = scale(v, 1. / n); point o = orthogonal(v);

	// Couleurs
	if (selected) {
		color = CYAN;
		drawBox(s, p1, color);
		drawBox(s, p2, color);
	}
	else if (!isIcon) color = getColor(p1.V);

	if (isIcon) for (int k = 0; k <= n; k++) line(p1, v, k, w, h, pxls, color);
	else for (int k = 0; k <= n; k++) line3(p1, v, o, k, w, h, pxls, color);
}
void drawG(screen s, point p1, point p2, Uint32 color, int selected, int isIcon) {
	SDL_Surface* win = s.w; int w = win->w, h = win->h; Uint32* pxls = win->pixels;
	int slotx, sloty;

	if (isIcon) s.zoom = 10;
	else {
		p1 = getScreenPoint(s, p1);
		p2 = getScreenPoint(s, p2);
	}
	point v = sum(p2, p1, -1), p; float n = norm(v); v = scale(v, 1. / n); point o = orthogonal(v);

	int b_inf = (n - s.zoom/2) / 2, b_sup = (n + s.zoom/2) / 2;

	// Gestion des couleurs
	Uint32 color1, color2;
	if (selected) {
		color1 = color2 = color = CYAN;
		drawBox(s, p1, color);
		drawBox(s, p2, color);
	}
	else if (isIcon) color1 = color2 = color;
	else {
		color1 = getColor(p1.V);
		color2 = getColor(p2.V);
	}

	int l = isIcon ? 1 : 3;
	// Minus icon
	point p_pm = sum(sum(p1, scale(v, b_inf - 10), 1), scale(o, s.zoom / 2), isIcon ? -1 : 1);
	for (int k = -l; k <= l; k++) {
		p = setPoint(p_pm.x + k, p_pm.y);
		if (p.x >= 0 && p.x < w && p.y >= 0 && p.y < h)
			*(pxls + (int)p.x + (int)p.y * w) = color;
	}

	// Plus icon
	p_pm = sum(sum(p1, scale(v, b_sup + 10), 1), scale(o, s.zoom / 2), isIcon ? -1 : 1);
	for (int k = -l; k <= l; k++) {
		p = setPoint(p_pm.x + k, p_pm.y);
		if (p.x >= 0 && p.x < w && p.y >= 0 && p.y < h)
			*(pxls + (int)p.x + (int)p.y * w) = color;
		p = setPoint(p_pm.x, p_pm.y + k);
		if (p.x >= 0 && p.x < w && p.y >= 0 && p.y < h)
			*(pxls + (int)p.x + (int)p.y * w) = color;
	}

	// Première patte
	if (isIcon) for (int k = 0; k < b_inf; k++) line(p1, v, k, w, h, pxls, color1);
	else for (int k = 0; k < b_inf; k++) line3(p1, v, o, k, w, h, pxls, color1);
	
	p = sum(sum(p1, scale(v, b_inf), 1), scale(o, s.zoom / 2), -1);
	if (isIcon) for (int k = 0; k < s.zoom; k++) line(p, o, k, w, h, pxls, color1);
	else for (int k = 0; k < s.zoom; k++) line3(p, o, v, k, w, h, pxls, color1);

	p = sum(sum(p1, scale(v, b_sup), 1), scale(o, s.zoom), -1);
	if (isIcon) for (int k = 0; k < s.zoom * 2; k++) line(p, o, k, w, h, pxls, color2);
	else for (int k = 0; k < s.zoom * 2; k++) line3(p, o, v, k, w, h, pxls, color2);

	// Seconde patte
	if (isIcon) for (int k = b_sup; k <= n; k++) line(p1, v, k, w, h, pxls, color2);
	else for (int k = b_sup; k <= n; k++) line3(p1, v, o, k, w, h, pxls, color2);
}
void drawAD(screen s, point p1, point p2, Uint32 color, int selected, int isIcon) {
	SDL_Surface* win = s.w; int w = win->w, h = win->h; Uint32* pxls = win->pixels;
	int slotx, sloty, slotxPrev1, slotyPrev1, slotxPrev2, slotyPrev2;

	if (isIcon) s.zoom = 10;
	else {
		p1 = getScreenPoint(s, p1);
		p2 = getScreenPoint(s, p2);
	}
	point v = sum(p2, p1, -1), p; float n = norm(v); v = scale(v, 1. / n); point o = orthogonal(v);

	int b_inf = (n - 1.2 * s.zoom) / 2, b_sup = (n + 1.2 * s.zoom) / 2;
	int l = b_sup - b_inf;

	// Gestion des couleurs
	Uint32 color1, color2;
	if (selected) {
		color1 = color2 = color = CYAN;
		drawBox(s, p1, color);
		drawBox(s, p2, color);
	}
	else if (isIcon) color1 = color2 = color;
	else {
		color1 = getColor(p1.V);
		color2 = getColor(p2.V);
	}	

	// Première patte
	if (isIcon) for (int k = 0; k < b_inf; k++) line(p1, v, k, w, h, pxls, color1);
	else for (int k = 0; k < b_inf; k++) line3(p1, v, o, k, w, h, pxls, color1);

	float sine, radius = (n / 2 - b_inf);

	slotxPrev1 = slotxPrev2 = p1.x + v.x * b_inf;
	slotyPrev1 = slotyPrev2 = p1.y + v.y * b_inf;
	if (isIcon) for (int k = b_inf; k < b_sup; k++) {
		sine = sin(acos((n / 2 - k) / radius)) * radius;
		slotx = p1.x + v.x * k + sine * o.x;
		sloty = p1.y + v.y * k + sine * o.y;
		//if (slotx >= 0 && slotx < w && sloty >= 0 && sloty < h) *(pxls + slotx + sloty * w) = color1;
		drawLine(setPoint(slotx, sloty), setPoint(slotxPrev1, slotyPrev1), w, h, pxls, color1);
		slotxPrev1 = slotx;
		slotyPrev1 = sloty;

		
		slotx -= 2 * sine * o.x;
		sloty -= 2 * sine * o.y;
		//if (slotx >= 0 && slotx < w && sloty >= 0 && sloty < h) *(pxls + slotx + sloty * w) = color1;
		drawLine(setPoint(slotx, sloty), setPoint(slotxPrev2, slotyPrev2), w, h, pxls, color1);
		slotxPrev2 = slotx;
		slotyPrev2 = sloty;
	}
	else for (int k = b_inf; k < b_sup; k++) {
		sine = sin(acos((n / 2 - k) / radius)) * radius;
		Uint32 c = addColor(scaleColor(color1, (float)(b_sup - k) / l), scaleColor(color2, (float)(k - b_inf) / l));

		slotx = p1.x + v.x * k + sine * o.x;
		sloty = p1.y + v.y * k + sine * o.y;
		if (slotx >= 0 && slotx < w && sloty >= 0 && sloty < h) {
			*(pxls + slotx + sloty * w) = c;
			*(pxls + slotx + 1 + sloty * w) = c;
			*(pxls + slotx - 1 + sloty * w) = c;
			*(pxls + slotx + (sloty + 1) * w) = c;
			*(pxls + slotx + (sloty - 1) * w) = c;
		}
		slotx -= 2 * sine * o.x;
		sloty -= 2 * sine * o.y;
		if (slotx >= 0 && slotx < w && sloty >= 0 && sloty < h) {
			*(pxls + slotx + sloty * w) = c;
			*(pxls + slotx + 1 + sloty * w) = c;
			*(pxls + slotx - 1 + sloty * w) = c;
			*(pxls + slotx + (sloty + 1) * w) = c;
			*(pxls + slotx + (sloty - 1) * w) = c;
		}
	}

	// Seconde patte
	if (isIcon) for (int k = b_sup; k <= n; k++) line(p1, v, k, w, h, pxls, color2);
	else for (int k = b_sup; k <= n; k++) line3(p1, v, o, k, w, h, pxls, color2);
}
void drawR(screen s, point p1, point p2, Uint32 color, int selected, int isIcon) {
	SDL_Surface* win = s.w; int w = win->w, h = win->h; Uint32* pxls = win->pixels;
	int slotx, sloty;

	if (isIcon) s.zoom = 8;
	else {
		p1 = getScreenPoint(s, p1);
		p2 = getScreenPoint(s, p2);
	}
	point v = sum(p2, p1, -1), p; float n = norm(v); v = scale(v, 1. / n); point o = orthogonal(v);

	int b_inf = isIcon ? n / 6 : n / 2 - (n < 150 ? n / 3 : 50) * s.zoom / 25, b_sup = isIcon ? n * 5.f / 6 : n / 2 + (n < 150 ? n / 3 : 50) * s.zoom / 25;
	int l = b_sup - b_inf;

	// Gestion des couleurs
	Uint32 color1, color2;
	if (selected) {
		color1 = color2 = color = CYAN;
		drawBox(s, p1, color);
		drawBox(s, p2, color);
	}
	else if (isIcon) color1 = color2 = color;
	else {
		color1 = getColor(p1.V);
		color2 = getColor(p2.V);
	}

	// Première patte
	if (isIcon) for (int k = 0; k < b_inf; k++) line(p1, v, k, w, h, pxls, color);
	else for (int k = 0; k < b_inf; k++) line3(p1, v, o, k, w, h, pxls, color1);

	p = sum(sum(p1, scale(v, b_inf), 1), scale(o, s.zoom / 2), -1);
	if (isIcon) for (int k = 0; k <= s.zoom; k++) line(p, o, k, w, h, pxls, color);
	else for (int k = 0; k < s.zoom; k++) line3(p, o, v, k, w, h, pxls, color1);

	if (isIcon) for (int k = -1; k < l + 2; k++) line(p, v, k, w, h, pxls, color);
	else for (int k = -1; k < l + 2; k++)
		line3(p, v, o, k, w, h, pxls, addColor(scaleColor(color1, (float)(l + 1 - k) / (l + 2)), scaleColor(color2, (float)(k + 1) / (l + 2))));
	p = sum(p, scale(o, s.zoom), 1);
	if (isIcon) for (int k = -1; k < l + 2; k++) line(p, v, k, w, h, pxls, color);
	else for (int k = -1; k < l + 2; k++)
		line3(p, v, o, k, w, h, pxls, addColor(scaleColor(color1, (float)(l + 1 - k) / (l + 2)), scaleColor(color2, (float)(k + 1) / (l + 2))));

	p = sum(sum(p1, scale(v, b_sup), 1), scale(o, s.zoom / 2), -1);
	if (isIcon) for (int k = 0; k <= s.zoom; k++) line(p, o, k, w, h, pxls, color);
	else for (int k = 0; k < s.zoom; k++) line3(p, o, v, k, w, h, pxls, color2);

	// Seconde patte
	if (isIcon) for (int k = b_sup; k <= n; k++) line(p1, v, k, w, h, pxls, color);
	else for (int k = b_sup; k <= n; k++) line3(p1, v, o, k, w, h, pxls, color2);
}
void drawL(screen s, point p1, point p2, Uint32 color, int selected, int isIcon) {}
void drawC(screen s, point p1, point p2, Uint32 color, int selected, int isIcon) {

	SDL_Surface* win = s.w; int w = win->w, h = win->h; Uint32* pxls = win->pixels;
	int slotx, sloty;

	if (isIcon) s.zoom = 12;
	else {
		p1 = getScreenPoint(s, p1);
		p2 = getScreenPoint(s, p2);
	}
	point v = sum(p2, p1, -1), p; float n = norm(v); v = scale(v, 1. / n); point o = orthogonal(v);

	int b_inf = (n - s.zoom) / 2 + 3, b_sup = (n + s.zoom) / 2 - 3;

	// Gestion des couleurs
	Uint32 color1, color2;
	if (selected) {
		color1 = color2 = color = CYAN;
		drawBox(s, p1, color);
		drawBox(s, p2, color);
	}
	else if (isIcon) color1 = color2 = color;
	else {
		color1 = getColor(p1.V);
		color2 = getColor(p2.V);
	}

	float bound = isIcon ? s.zoom * 1.75 : s.zoom * 2;
	// Première patte
	if (isIcon) for (int k = 0; k < b_inf; k++) line(p1, v, k, w, h, pxls, color1);
	else for (int k = 0; k < b_inf; k++) line3(p1, v, o, k, w, h, pxls, color1);
	
	p = sum(sum(p1, scale(v, b_inf), 1), scale(o, bound/2), -1);
	if (isIcon) for (int k = 0; k < bound; k++) line(p, o, k, w, h, pxls, color1);
	else for (int k = 0; k < bound; k++) line3(p, o, v, k, w, h, pxls, color1);

	p = sum(sum(p1, scale(v, b_sup), 1), scale(o, bound/2), -1);
	if (isIcon) for (int k = 0; k < bound; k++) line(p, o, k, w, h, pxls, color2);
	else for (int k = 0; k < bound; k++) line3(p, o, v, k, w, h, pxls, color2);

	// Seconde patte
	if (isIcon) for (int k = b_sup; k <= n; k++) line(p1, v, k, w, h, pxls, color2);
	else for (int k = b_sup; k <= n; k++) line3(p1, v, o, k, w, h, pxls, color2);

}
void drawD(screen s, point p1, point p2, Uint32 color, int selected, int isIcon) {}
void drawVCC(screen s, point p1, point p2, Uint32 color, int selected, int isIcon) {
	SDL_Surface* win = s.w; int w = win->w, h = win->h; Uint32* pxls = win->pixels;
	int slotx, sloty;

	if (isIcon) s.zoom = 10;
	else {
		p1 = getScreenPoint(s, p1);
		p2 = getScreenPoint(s, p2);
	}
	point v = sum(p2, p1, -1), p; float n = norm(v); v = scale(v, 1. / n); point o = orthogonal(v);
	int z = isIcon ? n / 2 : (s.zoom < 25 ? s.zoom : 20);
	int b_inf = n - z;

	// Gestion des couleurs
	Uint32 color1;
	if (selected) {
		color1 = color = CYAN;
		drawBox(s, p1, color);
	}
	else color1 = isIcon ? color : getColor(p1.V);

	if (isIcon) for (int k = 0; k < b_inf; k++) line(p1, v, k, w, h, pxls, color1);
	else for (int k = 0; k < b_inf; k++) line3(p1, v, o, k, w, h, pxls, color1);
	p1 = sum(p1, scale(v, b_inf), 1);
	point p_1 = sum(p1, scale(o, z),-1);
	point p_2 = sum(p1, scale(o, z), 1);
	if (isIcon) {
		drawLine(p_1, p_2, w, h, pxls, color);
		drawLine(p_1, p2, w, h, pxls, color);
		drawLine(p_2, p2, w, h, pxls, color);
	}
	else {
		drawLine3(p_1, p_2, w, h, pxls, color);
		drawLine3(p_1, p2, w, h, pxls, color);
		drawLine3(p_2, p2, w, h, pxls, color);
	}

}
void drawGND(screen s, point p1, point p2, Uint32 color, int selected, int isIcon) {
	SDL_Surface* win = s.w; int w = win->w, h = win->h; Uint32* pxls = win->pixels;
	int slotx, sloty;

	if (isIcon) s.zoom = 10;
	else {
		p1 = getScreenPoint(s, p1);
		p2 = getScreenPoint(s, p2);
	}
	point v = sum(p2, p1, -1), p; float n = norm(v); v = scale(v, 1. / n); point o = orthogonal(v);
	int z = isIcon ? n / 2 : (s.zoom < 25 ? s.zoom : 20);
	int b_inf = n - z;

	// Gestion des couleurs
	Uint32 color1;
	if (selected) {
		color1 = color = CYAN;
		drawBox(s, p1, color);
	}
	else color1 = isIcon ? color : getColor(p1.V);

	if (isIcon) for (int k = 0; k < b_inf; k++) line(p1, v, k, w, h, pxls, color1);
	else for (int k = 0; k < b_inf; k++) line3(p1, v, o, k, w, h, pxls, color1);
	p1 = sum(p1, scale(v, b_inf), 1);
	point p_1 = sum(p1, scale(o, z), -1);
	point p_2 = sum(p1, scale(o, z), 1);
	point v_1 = sum(p2, p_1, -1);
	point v_2 = sum(p2, p_2, -1);
	if (isIcon) {
		drawLine(p_1, p_2, w, h, pxls, color);
		drawLine(sum(p_2, scale(v_2, 0.4), 1), sum(p_1, scale(v_1, 0.4), 1), w, h, pxls, color1);
		drawLine(sum(p_2, scale(v_2, 0.8), 1), sum(p_1, scale(v_1, 0.8), 1), w, h, pxls, color1);
	}
	else {
		drawLine3(p_1, p_2, w, h, pxls, color);
		drawLine3(sum(p_2, scale(v_2, 0.4), 1), sum(p_1, scale(v_1, 0.4), 1), w, h, pxls, color1);
		drawLine3(sum(p_2, scale(v_2, 0.8), 1), sum(p_1, scale(v_1, 0.8), 1), w, h, pxls, color1);
	}
}

void line(point p, point v, int k, int w, int h, Uint32* pxls, Uint32 color) {
	int slotx, sloty;

	slotx = p.x + (int)(v.x * k);
	sloty = p.y + (int)(v.y * k);
	*(pxls + slotx + sloty * w) = color;
	*(pxls + slotx + 1 + sloty * w) = color;
}
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
void drawLine(point p1, point p2, int w, int h, Uint32* pxls, Uint32 color) {
	point v = sum(p2, p1, -1);
	float n = norm(v);
	v = scale(v, 1.f / n);

	int slotx, sloty;
	for (int k = 0; k <= n; k++) {
		slotx = p1.x + (int)(v.x * k);
		sloty = p1.y + (int)(v.y * k);
		if (slotx < 0 || sloty < 0) continue;
		*(pxls + slotx + sloty * w) = color;
		//*(pxls + slotx + 1 + sloty * w) = color;
	}
}
void drawLine3(point p1, point p2, int w, int h, Uint32* pxls, Uint32 color) {
	point v = sum(p2, p1, -1);
	float n = norm(v);
	v = scale(v, 1.f / n);
	point o = orthogonal(v);

	int slotx, sloty;
	for (int k = 0; k <= n; k++) {
		slotx = p1.x + v.x * k;
		sloty = p1.y + v.y * k;
		if (slotx >= 0 && slotx < w && sloty >= 0 && sloty < h) *(pxls + slotx + sloty * w) = color;

		slotx = p1.x + v.x * k + o.x;
		sloty = p1.y + v.y * k + o.y;
		if (slotx >= 0 && slotx < w && sloty >= 0 && sloty < h) *(pxls + slotx + sloty * w) = color;

		slotx = p1.x + v.x * k - o.x;
		sloty = p1.y + v.y * k - o.y;
		if (slotx >= 0 && slotx < w && sloty >= 0 && sloty < h) *(pxls + slotx + sloty * w) = color;
	}
	
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

void drawSelectedArea(SDL_Surface* s, point p1, point p2) {
	int w = s->w, h = s->h;
	Uint32* pxls = s->pixels;
	if (p2.x < 0) p2.x = 0;
	if (p2.y < 0) p2.y = 0;
	if (p2.x >= w) p2.x = w - 1;
	if (p2.y >= h) p2.y = h - 1;

	int ward = p1.x > p2.x ? -1 : 1;
	for (int x = p1.x; x != p2.x; x += ward) {
		*(pxls + x + (int)p1.y * w) = WHITE;
		*(pxls + x + (int)p2.y * w) = WHITE;
	}
	ward = p1.y > p2.y ? -1 : 1;
	for (int y = p1.y; y != p2.y; y += ward) {
		*(pxls + (int)p1.x + y * w) = WHITE;
		*(pxls + (int)p2.x + y * w) = WHITE;
	}
}

void drawGraph(screen s, elec* e, float Ttmp) {
	graph* g = s.g;

	if (g->tmpLast == (int)Ttmp && !g->toDraw) return;
	g->toDraw = 0;

	int w = s.w->w, h = s.w->h, i, j, heigth = 154, ypos = h - heigth, y0 = ypos + 77;
	Uint32* pxls = s.w->pixels;

	// Contours
	for (i = ypos; i < h; i++) *(pxls + w - 1 + i * w) = WHITE;
	for (j = 51; j < w; j++) {
		*(pxls + j + (h - 1) * w) = WHITE;
		*(pxls + j + ypos * w) = WHITE;
		*(pxls + j + (ypos - 1) * w) = GREY;
		*(pxls + j + (ypos - 2) * w) = DGREY;
	}

	// Remplissage intérieur
	{
		for (i = 51; i < w - 147; i++)
			for (j = ypos + 1; j < ypos + 7; j++)
				*(pxls + i + j * w) = *(pxls + i + (j + 146) * w) = 20;

		for (j; j < h - 7; j++) {
			for (i = 51; i < 66; i++)
				*(pxls + i + j * w) = 20;
			for (i = w - 165; i < w - 1; i++)
				*(pxls + i + j * w) = 20;
		}

		// Contours
		for (i = 66; i < w - 163; i++)
			*(pxls + i + (ypos + 7) * w) = *(pxls + i + (ypos + 147) * w) = LGREY;
		for (j = ypos + 8; j < ypos + 147; j++)
			*(pxls + 66 + j * w) = *(pxls + w - 164 + j * w) = LGREY;

		for (i = 67; i < w - 164; i++)
			for (j = ypos + 8; j < ypos + 146; j++)
				*(pxls + i + j * w) = 0;
	}

	// Graduations
	{
		// Abscisse
		for (i = 67; i < w - 164; i++)
			*(pxls + i + y0 * w) = LGREY;
		for (i = 67; i < w - 164; i += g->secPixRatio)
			*(pxls + i + (y0 - 1) * w) = LGREY;

		// Ordonnée
		for (j = y0 + 5 * g->UpixRatio; j < ypos + 147; j += 5 * g->UpixRatio) {
			*(pxls + 67 + j * w) = LGREY;
			for (i = 68; i < w - 164; i++)
				*(pxls + i + j * w) = DGREY;
		}
		for (j = y0 - 5 * g->UpixRatio; j > ypos + 8; j -= 5 * g->UpixRatio) {
			*(pxls + 67 + j * w) = LGREY;
			for (i = 68; i < w - 164; i++)
				*(pxls + i + j * w) = DGREY;
		}
	}

	if (g->tmpLast != (int)Ttmp) {
		g->tmpLast = (int)Ttmp;

		g->Utab[g->index % g->tabLength] = e->U;
		g->Itab[g->index % g->tabLength] = e->I;
		g->index++;
	}
	
	for (i = 1; i < g->index % g->tabLength; i++) {
		drawLine(
			setPoint(67 + (i - 1) * g->secPixRatio / 10, y0 - g->Utab[i - 1] * g->UpixRatio),
			setPoint(67 + i * g->secPixRatio / 10, y0 - g->Utab[i] * g->UpixRatio),
			w, h, pxls, LRED);
		drawLine(
			setPoint(67 + (i - 1) * g->secPixRatio / 10, y0 - g->Itab[i - 1] * g->IpixRatio),
			setPoint(67 + i * g->secPixRatio / 10, y0 - g->Itab[i] * g->IpixRatio),
			w, h, pxls, LBLUE);
	}

	// Graph Dim on va dire (w - 210)x140 à partir de (55;155)


}

void drawParamBar(screen s, elec* e) {
	int w = s.w->w, h = s.w->h, i, j, larg = 150, xpos = w - larg;
	Uint32* pxls = s.w->pixels;

	// Contours
	for (i = xpos; i < w; i++) *(pxls + i) = WHITE;
	for (j = 0; j < h - 156; j++) {
		*(pxls + w - 1 + j * w) = WHITE;
		*(pxls + xpos + 2 + j * w) = WHITE;
		*(pxls + xpos + 1 + j * w) = GREY;
		*(pxls + xpos + j * w) = DGREY;
	}

	// Remplissage intérieur
	for (i = w - larg + 2; i < w - 1; i++)
		for (j = 1; j < h - 154; j++)
			*(pxls + i + j * w) = 20;

	/*
	TTF_Font* Sans = TTF_OpenFont("Sans.ttf", 24);
	SDL_Color White = { 255, 255, 255 };
	SDL_Surface* surfaceMessage = TTF_RenderText_Solid(Sans, "put your text here", White);
	SDL_Texture* Message = SDL_CreateTextureFromSurface(s.w, surfaceMessage);

	SDL_Rect Message_rect; //create a rect
	Message_rect.x = 0;  //controls the rect's x coordinate 
	Message_rect.y = 0; // controls the rect's y coordinte
	Message_rect.w = 100; // controls the width of the rect
	Message_rect.h = 100; // controls the height of the rect
	SDL_RenderCopy(s.w, Message, NULL, &Message_rect);*/

}

void drawGUI(screen s, icon* I) {
	int w = s.w->w, h = s.w->h, i, j;
	Uint32* pxls = s.w->pixels;

	// Contours
	for (i = 0; i < 50; i++) *(pxls + i) = *(pxls + i + (h - 1) * w) = WHITE;
	for (j = 0; j < h; j++) {
		*(pxls + j * w) = WHITE;
		*(pxls + 48 + j * w) = WHITE;
		*(pxls + 49 + j * w) = GREY;
		*(pxls + 50 + j * w) = DGREY;
	}
	// Remplissage intérieur
	for (i = 1; i < 48; i++)
		for (j = 1; j < h - 1; j++)
			*(pxls + i + j * w) = 20;

	while (I) {
		drawGUIBox(s, I);
		I = I->next;
	}
}

void drawGUIBox(screen s, icon* i) {

	int size = i->size, z = i->z;
	int w = s.w->w, dec = 25 - size, size2 = 2 * size + dec;
	Uint32* pxls = s.w->pixels;

	if (i->pressed) {
		for (int i = 0; i < 2 * size; i++) {
			*(pxls + i + dec + (size + z) * w) = DGREY;
			*(pxls + size2 + (z - size + i) * w) = DGREY;

			*(pxls + i + dec + (z - size + 1) * w) = DGREY;
			*(pxls + dec + 1 + (z - size + i) * w) = DGREY;

			*(pxls + i + dec + (z - size) * w) = WHITE;
			*(pxls + dec + (z - size + i) * w) = WHITE;
		}
	}
	else {
		for (int i = 0; i < 2 * size; i++) {
			*(pxls + i + dec + (size + z - 1) * w) = WHITE;
			*(pxls + size2 - 1 + (z - size + i) * w) = WHITE;

			*(pxls + i + dec + (size + z) * w) = LGREY;
			*(pxls + size2 + (z - size + i) * w) = LGREY;

			*(pxls + i + dec + (z - size) * w) = DGREY;
			*(pxls + dec + (z - size + i) * w) = DGREY;
		}
		*(pxls + size2 + (size + z) * w) = LGREY;
	}

	float p = size / 3;
	draw(s, setPoint(p + dec + i->pressed, z - 2 * p + i->pressed), setPoint(p * 5 + dec + i->pressed, z + 2 * p + i->pressed), i->t, i->pressed ? CYAN : WHITE, 0, 1);

}

// Interactions
void selectZone(screen s, list* L, point p1, point p2) {
	p1 = setPoint((p1.x + s.offsetx) / s.zoom, (p1.y + s.offsety) / s.zoom);
	p2 = setPoint((p2.x + s.offsetx) / s.zoom, (p2.y + s.offsety) / s.zoom);
	float x1, x2, y1, y2;
	if (p1.x < p2.x) {x1 = p1.x; x2 = p2.x;}
	else {x1 = p2.x; x2 = p1.x;}
	if (p1.y < p2.y) {y1 = p1.y; y2 = p2.y;}
	else { y1 = p2.y; y2 = p1.y; }

	elec* e;
	point p1_, p2_;
	while (L) {
		e = L->e;
		p1_ = *e->p1;
		p2_ = *e->p2;

		if (p1_.x > x1 && p1_.x < x2 && p1_.y > y1 && p1_.y < y2 &&
			p2_.x > x1 && p2_.x < x2 && p2_.y > y1 && p2_.y < y2) e->selected = 1;
		else e->selected = 0;

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

		if (0 && e->t == WIRE) {
			rp = getSimuPoint(s, p, 0);
			point ve = sum(*e->p1, *e->p2, -1), vp = sum(*e->p1, rp, -1);
			if (norm(sum(p, getScreenPoint(s, rp), -1)) < 8. && (ve.x == 0 && vp.x == 0 || ve.y / ve.x == vp.y / vp.x)) {
				drawBox(s, getScreenPoint(s, rp), WHITE);
				return NULL;
			}
		}

		rp = getScreenPoint(s, *e->p1);
		n = norm(sum(p, rp, -1));
		if (n < 8.) return e->p1;

		if (e->t > GND) {
			rp = getScreenPoint(s, *e->p2);
			n = norm(sum(p, rp, -1));
			if (n < 8.) return e->p2;
		}

		L = L->next;
	}
	return NULL;
}
type selectType(icon* I, point p) {
	type t = -1;
	icon* Itmp = I;
	while (I) {
		if (abs(3 + I->size - p.x) < I->size && abs(I->z - p.y) < I->size) {
			unpressAll(Itmp);
			I->pressed = 1;
			t = I->t;
		}
		I = I->next;
	}
	return t;
}
void deselectAll(list* L) {
	while (L) {
		L->e->selected = 0;
		L = L->next;
	}
}
void unpressAll(icon* I) {
	while (I) {
		I->pressed = 0;
		I = I->next;
	}
}

// Simulation
void drawCurrent(screen s, list* L, int T) {
	SDL_Surface* win = s.w; int w = win->w, h = win->h; Uint32* pxls = win->pixels;

	elec* e;
	point p1, p2, v;
	float I, n;
	int slotx, sloty, start;
	while (L) {
		e = L->e; p1 = getScreenPoint(s, *e->p1); p2 = getScreenPoint(s, *e->p2); I = -enterOrExitNode(e->p1);
		v = sum(p2, p1, -1);
		n = norm(v); v = scale(v, 1.f / n);
		start = (int)(200 * I * T)%25;
		if (start < 0) start += 25;

		for (int k = start; k <= n - start; k += 25) {
			slotx = p1.x + (int)(v.x * k) - 2;
			sloty = p1.y + (int)(v.y * k) - 2;
			for (int i = 0; i < 5; i++) {
				for (int j = 0; j < 5; j++) {
					if (slotx >= 0 && slotx < w && sloty >= 0 && sloty < h) *(pxls + slotx + sloty * w) = YELLOW;
					sloty++;
				}
				slotx++;
				sloty-=5;
			}
		}

		L = L->next;
	}
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
Uint32 getColor(float V) {
	int Vp = 127 * V / 12;
	return (128 - Vp) << 16 | (128 + Vp) << 8 | 128 - abs(Vp);
}
Uint32 addColor(Uint32 color1, Uint32 color2) {
	Uint8* c1 = (Uint8*)&color1;
	Uint8* c2 = (Uint8*)&color2;
	Uint8 c[3];
	int tmpc;

	c[0] = (tmpc = (int)c1[0] + (int)c2[0]) > 255 ? 255 : tmpc;
	c[1] = (tmpc = (int)c1[1] + (int)c2[1]) > 255 ? 255 : tmpc;
	c[2] = (tmpc = (int)c1[2] + (int)c2[2]) > 255 ? 255 : tmpc;

	return c[0] << 16 | c[1] << 8 | c[2];

}
Uint32 scaleColor(Uint32 color, float n) {
	Uint8* scaledColor = (Uint8*)&color;
	int tmpc;
	scaledColor[0] = (tmpc = (int)scaledColor[0] * n) < 0 ? 0 : tmpc > 255 ? 255 : tmpc;
	scaledColor[1] = (tmpc = (int)scaledColor[1] * n) < 0 ? 0 : tmpc > 255 ? 255 : tmpc;
	scaledColor[2] = (tmpc = (int)scaledColor[2] * n) < 0 ? 0 : tmpc > 255 ? 255 : tmpc;
	return scaledColor[0] << 16 | scaledColor[1] << 8 | scaledColor[2];
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