#include "graph.h"

#define max(a,b) a > b ? a : b

void drawJulia(SDL_Surface* s, pointf offset, double UPP, pointf c, float power, int nb_iter) {

	int w = s->w, h = s->h;
	Uint32* pxls = s->pixels;

	double offset_x = offset.x - w * UPP / 2.f;
	double offset_y = -offset.y - h * UPP / 2.f;

	// Rayon d'échappement
	float roof = max(2, norm(c) + 1);

	double cx = c.x, cy = c.y;

	int y;
	#pragma omp parallel for schedule(dynamic)
	for (y = 0; y < h; y++) {
		double zy = y * UPP + offset_y;

		for (int x = 0; x < w; x++) {
			double zx = x * UPP + offset_x;
			int val = iterateJulia(zx, zy, cx, cy, nb_iter);

			*(pxls + x + y * w) = getColor(val, nb_iter);
		}
	}
	
	/*for (int y = 0; y < h; y++) {
	//	for (int x = 0; x < w; x++) {

	//		pointf z = sumf(scalef((pointf) { x, y }, UPP), offset, 1);

	//		val = iterate(z, c, power, nb_iter, roof);
	//		*(pxls + x + y * w) = getColor(val, nb_iter);

	//	}
	}*/

}

void drawMandelbrot(SDL_Surface* s, pointf offset, double UPP, float power, int nb_iter) {

	int w = s->w, h = s->h;
	Uint32* pxls = s->pixels;

	double offset_x = offset.x - w * UPP / 2.f;
	double offset_y = -offset.y - h * UPP / 2.f;

	int y;
	#pragma omp parallel for schedule(dynamic)
	for (y = 0; y < h; y++) {
		double cy = y * UPP + offset_y;

		for (int x = 0; x < w; x++) {
			double cx = x * UPP + offset_x;
			int val = iterateMandelbrot(cx, cy, nb_iter);

			*(pxls + x + y * w) = getColor(val, nb_iter);
		}
	}

}

void drawAxis(SDL_Surface* s, pointf offset, double UPP) {

	int w = s->w, h = s->h;
	Uint32* pxls = s->pixels, *pxlsTmp;

	float PPU = 1.f / UPP;
	offset = scalef(offset, PPU);

	// Origin coordinates
	int X = w / 2 - offset.x;
	int Y = h / 2 + offset.y;

	if (Y > 0 && Y < h) {

		// Horizontal Axis
		pxlsTmp = pxls + Y * w;
		for (int x = 0; x < w; x++) {
			invertColor(pxlsTmp);
			pxlsTmp++;
		}

		// Graduations
		int max_graduations_x = w / PPU;
		int start_x = X % (int)PPU;

		pxlsTmp = pxls + start_x + Y * w;
		for (int i = 0; i < max_graduations_x; i++) {

			if (start_x + i * PPU == X) { // Avoids inverting color at origin
				pxlsTmp += (int)PPU;
				continue;
			}

			if (Y < h - 1) invertColor(pxlsTmp + w);
			if (Y < h - 2) invertColor(pxlsTmp + 2 * w);
			if (Y > 1) invertColor(pxlsTmp - w);
			if (Y > 2) invertColor(pxlsTmp - 2 * w);
			pxlsTmp += (int)PPU;
		}

	}

	if (X > 0 && X < w) {

		// Vertical Axis
		pxlsTmp = pxls + X;
		for (int y = 0; y < h; y++) {
			invertColor(pxlsTmp);
			pxlsTmp += w;
		}

		// Graduations
		int max_graduations_y = h / PPU;
		int start_y = Y % (int)PPU;

		pxlsTmp = pxls + X + start_y * w;
		for (int i = 0; i < max_graduations_y; i++) {
			if (start_y + i * PPU == Y) { // Avoids inverting color at origin
				pxlsTmp += (int)PPU * w;
				continue;
			}

			if (X < w - 1) invertColor(pxlsTmp + 1);
			if (X < w - 2) invertColor(pxlsTmp + 2);
			if (X > 1)invertColor(pxlsTmp - 1);
			if (X > 2)invertColor(pxlsTmp - 2);
			pxlsTmp += (int)PPU * w;
		}
	}

}

void drawC(SDL_Surface* s, pointf offset, double UPP, pointf c) {
	int w = s->w, h = s->h;
	Uint32* pxls = s->pixels;

	int c_x = (c.x - offset.x) / UPP + w / 2.f;
	int c_y = (c.y + offset.y) / UPP + h / 2.f;

	if (c_y > 0 && c_y < h)
		for (int x = c_x - 3; x <= c_x + 3; x++)
			if (x > 0 && x < w) *(pxls + x + c_y * w) = C_RED;

	if (c_x > 0 && c_x < w)
		for (int y = c_y - 3; y <= c_y + 3; y++)
			if (y > 0 && y < h) *(pxls + c_x + y * w) = C_RED;
}

void drawCursor(SDL_Surface* s) {
	int w = s->w, h = s->h;
	int w2 = w/2, h2 = h/2;
	Uint32* pxls = s->pixels;

	int radius = 8;
	int half_width = 1;

	for (int y = h2 - half_width; y <= h2 + half_width; y++)
		for (int x = w2 - radius; x <= w2 + radius; x++)
			if (abs(x - w2) >= half_width)
				*(pxls + x + y * w) = abs(y - h2) < half_width && abs(x - w2) < radius ? C_WHITE : C_BLACK; // Bordures en noir

	for (int x = w2 - half_width; x <= w2 + half_width; x++)
		for (int y = h2 - radius; y <= h2 + radius; y++)
			if (abs(y - h2) >= half_width)
				*(pxls + x + y * w) = abs(x - w2) < half_width && abs(y - h2) < radius ? C_WHITE : C_BLACK; // Bordures en noir

}

void invertColor(Uint32* color) {
	Uint8* c = (Uint8*) color;
	c[0] = 0xFF - c[0];
	c[1] = 0xFF - c[1];
	c[2] = 0xFF - c[2];
	c[3] = 0xFF;
}

Uint32 getColor(int val, int val_max) {
	if (val == val_max) return C_BLACK;
	//else return C_WHITE;

	float arg = val * PI / 15.f;
	return (int)(255 * cosf(arg)) << 16 | (int)(255 * cosf(arg + 2 * PI / 3)) << 8 | (int)(255 * cosf(arg + 4 * PI / 3));
}