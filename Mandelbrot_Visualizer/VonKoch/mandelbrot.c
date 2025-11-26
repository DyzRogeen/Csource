#include "mandelbrot.h"

int iterateJulia(double zx, double zy, double cx, double cy, int nb_iter) {

	double zx2, zy2, zxtmp;
	for (int i = 0; i < nb_iter; i++) {
		// z = z² + c
		zx2 = zx * zx;
		zy2 = zy * zy;

		// Si z sort de son rayon d'échappement, on considère que la suite diverge.
		if (zx2 + zy2 > 4.0) return i;

		zxtmp = zx2 - zy2 + cx;
		zy = 2.0 * zx * zy + cy;
		zx = zxtmp;
	}

	return nb_iter;

}

// Même principe que pour Julia sauf que c varie et z0 = 0
int iterateMandelbrot(double cx, double cy, int nb_iter) {

	// On peut sauter la première étape triviale car z0 = 0
	double zx = cx, zy = cy, zx2, zy2, zxtmp;
	for (int i = 0; i < nb_iter - 1; i++) {
		// z = z² + c
		zx2 = zx * zx;
		zy2 = zy * zy;

		// Si z sort de son rayon d'échappement, on considère que la suite diverge.
		if (zx2 + zy2 > 4.0) return i;

		zxtmp = zx2 - zy2 + cx;
		zy = 2.0 * zx * zy + cy;
		zx = zxtmp;
	}

	return nb_iter;

}

// Moins performant mais power réglable
int iterate(pointf z, pointf c, float power, int nb_iter, float roof) {

	//printf("Z start : %f + %f i  roof : %f\n", z.x, z.y, roof);
	for (int i = 0; i < nb_iter; i++) {
		// z = z^n + c
		z = sumf(powZ(z, power), c, 1);

		//printf("%d : Z : %f + %f i\n", i, z.x, z.y);

		// Si z sort de son rayon d'échappement, on considère que la suite diverge.
		if (norm(z) > roof) return i;
	}

	return nb_iter;

}

pointf powZ(pointf z, float power) {
	
	float n = norm(z);
	pointf v = scalef(z, 1.f / n);
	float arg = (v.y > 0 ? acosf(v.x) : PI - acosf(v.x)) * power;
	//printf("arg : %f ", arg);
	return scalef((pointf) { cosf(arg), sinf(arg) }, powf(n, power));

}


// Utils

point sum(point p1, point p2, int diff) {
	if (diff != -1) diff = 1;
	p2.x += p1.x * diff;
	p2.y += p1.y * diff;
	return p2;
}

pointf sumf(pointf p1, pointf p2, int diff) {
	if (diff != -1) diff = 1;
	p2.x += p1.x * diff;
	p2.y += p1.y * diff;
	return p2;
}

pointf scale(point v, float f) {
	return (pointf) { v.x * f , v.y * f };
}
pointf scalef(pointf v, float f) {
	v.x *= f;
	v.y *= f;
	return v;
}

float norm(pointf v) {
	return sqrtf(v.x * v.x + v.y * v.y);
}