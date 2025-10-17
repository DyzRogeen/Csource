#include "graph.h"

void drawGrid(Graph* g) {

	int w = g->width, h = g->height;
	Uint32* pxls = g->pixels;

	float zoom = g->zoom;
	float scaleX = g->scaleX / zoom;
	float scaleY = g->scaleY / zoom;

	int offsetX = g->offset.x;
	int offsetY = g->offset.y;

	int pxlRatioX = w / scaleX;
	int pxlRatioY = h / scaleY;

	offsetX = offsetX < 0 ? 0 : (offsetX >= w ? w - 1 : offsetX);
	offsetY = offsetY < 0 ? 0 : (offsetY >= h ? h - 1 : offsetY);

	// Grid
	int startX = offsetX % pxlRatioX;
	int startY = offsetY % pxlRatioY;
	
	for (int row = startY; row < h; row += pxlRatioY)
		for (int x = 0; x < w; x++) *(pxls + x + row * h) = DGREY;
	for (int col = startX; col < w; col += pxlRatioX)
		for (int y = 0; y < h; y++) *(pxls + col + y * w) = DGREY;

	// Axis
	for (int x = 0; x < w; x++) *(pxls + x + offsetY * w) = BLUE;
	for (int y = 0; y < h; y++) *(pxls + offsetX + y * w) = RED;

}

void drawCurve(Graph* g, point p_mouse, int tMax) {

	int w = g->width, h = g->height;
	Uint32* pxls = g->pixels;

	float zoom = g->zoom;
	float scaleX = g->scaleX / zoom;
	float scaleY = g->scaleY / zoom;

	int pxlRatioX = w / scaleX;
	int pxlRatioY = h / scaleY;

	int offsetX = g->offset.x;
	int offsetY = g->offset.y;

	pointf p = getCoordinates(p_mouse, pxlRatioX, pxlRatioY, offsetX, offsetY);

	system("cls");
	if (p.y < 0)	printf("s = [%.2f - i%.2f]\n\n", p.x, -p.y);
	else			printf("s = [%.2f + i%.2f]\n\n", p.x,  p.y);

	pointf pf, p_last, pTmp, pSum = { 0, 0 }, pI, pI_last = { 0, 0 };

	int div = 10;
	for (float t = 0; t < tMax; t+=1.f/div) {
		pTmp = dotMExp(func(t), p, t);
		pSum = addPoint(pSum, pTmp, 1);

		pf = getGraphCoordinates(pTmp, pxlRatioX, pxlRatioY, offsetX, offsetY);
		if (t > 0) {
			drawLine(pxls, w, h, pf, p_last, GREEN);
			
			if ((int)(t * div) % div == 0) {
				pI = addPoint(scale(pSum, 1.f/div), pI_last, 1);
				drawLine(pxls, w, h,
					getGraphCoordinates(pI, pxlRatioX, pxlRatioY, offsetX, offsetY),
					getGraphCoordinates(pI_last, pxlRatioX, pxlRatioY, offsetX, offsetY),
					ORANGE);
				pSum = (pointf){ 0, 0 };
				pI_last = pI;
			}
		}
		
		p_last = pf;
	}

	drawLine(pxls, w, h,
		(pointf){offsetX, offsetY},
		getGraphCoordinates(pI_last, pxlRatioX, pxlRatioY, offsetX, offsetY),
		WHITE);

	//printf("\n\n");

}

void drawLine(Uint32* pxls, int w, int h, pointf p1, pointf p2, Uint32 color) {

	// Retour si les deux points sont hors de l'écran
	if ((p1.x < 0 || p1.x >= w || p1.y < 0 || p1.y >= h) &&
		(p2.x < 0 || p2.x >= w || p2.y < 0 || p2.y >= h)) return;

	pointf v = addPoint(p2, p1, -1), ptmp;
	float n = norm(v);
	float lim = sqrtf(w * w + h * h);
	v = scale(v, 1.f / n);
	
	if (n > lim) n = lim;

	for (int i = 0; i < n; i++) {
		ptmp = addPoint(scale(v, i), p1, 1);
		if (ptmp.x < 0 || ptmp.x >= w || ptmp.y < 0 || ptmp.y >= h) continue;
		*(pxls + (int)ptmp.x + (int)(ptmp.y) * w) = color;
	}
}

pointf func(float t) {
	pointf res;
	res.x = cos(t);
	res.y = sin(t);
	return res;
}

pointf dotMExp(pointf pFunc, pointf p, float t) {
	pointf pexp;
	float cosPT = cos(p.y * t);
	float sinPT = sin(p.y * t);
	float exp = pow(E, -p.x * t);

	// f(t) * e^-(x + iy)t => (a + ib) * (cos(yt) - isin(yt)) * e^-xt
	pexp.x = (pFunc.x * cosPT + pFunc.y * sinPT) * exp;
	pexp.y = (pFunc.y * cosPT - pFunc.x * sinPT) * exp;
	return pexp;
}

// UTILS

pointf getCoordinates(point p, int ratioX, int ratioY, int offsetX, int offsetY) {
	pointf pf;
	pf.x = (float)(p.x - offsetX) / ratioX;
	pf.y = (float)(offsetY - p.y) / ratioY;
	return pf;
}

pointf getGraphCoordinates(pointf pf, int ratioX, int ratioY, int offsetX, int offsetY) {
	pointf p;
	p.x = pf.x * ratioX + offsetX;
	p.y = -pf.y * ratioY + offsetY;
	return p;
}

pointf addPoint(pointf p1, pointf p2, int diff) {
	if (diff != 1) diff = -1;

	p1.x += p2.x * diff;
	p1.y += p2.y * diff;
	return p1;
}

pointf scale(pointf v, float scale) {
	v.x *= scale;
	v.y *= scale;
	return v;
}

pointf orthonormal(pointf v) {
	return scale(v, 1.f / norm(v));
}

float norm(pointf v) {
	return sqrtf(v.x * v.x + v.y * v.y);
}

pointf toPointf(point p) {
	pointf pf;
	pf.x = p.x;
	pf.y = p.y;
	return pf;
}