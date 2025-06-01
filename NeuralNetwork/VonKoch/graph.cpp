#include "graph.h"

void drawNeuron(SDL_Surface* window, point p, float val, int size) {
	Uint32* pxls = (Uint32*)window->pixels;
	int w = window->w, h = window->h;
	int size2 = size * size, hsize, yslotP, yslotM;
	int px = p.x, py = p.y;

	Uint32 color, n_color = val < 0 ? 0 : val > 255 ? 255 : val; // TODO : centrer réduire

	for (int y = 0; y <= size; y++) {
		yslotP = (py + y) * w;
		yslotM = (py - y) * w;
		hsize = sqrt(size2 - y * y);
		for (int x = 0; x <= hsize; x++) {
			color = (x > hsize - 2 || y > size - 2) ? WHITE : n_color; // Blanc si contours
			*(pxls + px + x + yslotP) = color;
			*(pxls + px - x + yslotP) = color;
			*(pxls + px + x + yslotM) = color;
			*(pxls + px - x + yslotM) = color;
		}
	}

}

void drawLink(SDL_Surface* window, point p1, point p2, int size, float w) {

	Uint32* pxls = (Uint32*)window->pixels;
	int width = window->w;

	pointf v = sumPointf(setPointf(p1.x + size, p1.y), setPointf(p2.x - size, p2.y), -1);
	float l = norm(v);
	v = scale(v, 1.0 / l);

	int ctmp;
	Uint32 color = w > 0 ?
		(ctmp = 255 / w) > BLUE ? BLUE : ctmp < 0 ? 0 : ctmp :
		(ctmp = -(255 << 16) / w) > RED ? RED : ctmp < 0 ? 0 : ctmp;

	float x = p1.x + size, y = p1.y;
	for (int i = 0; i < l; i++) {
		*(pxls + (int)(x + y * width)) = color;
		x += v.x;
		y += v.y;
	}

}

void drawNetwork(SDL_Surface* window, NeuralNetwork n) {
	Uint32* pxls = (Uint32*)window->pixels;
	int w = window->w, h = window->h;

	LayerList* ll = n.getLayers();
	int networkW = n.getSize();
	int networkH = getMaxLayerSize(ll);

	// (nsize + ssize) * networkW + nsize - ssize = (networkW + 1) * nsize + (networkW - 1) * ssize = w
	// (nsize + ssize/6) * networkH + nsize - ssize / 6 = (networkH + 1) * nsize + (networkH - 1/6) * ssize = h
	// <=> ssize = (w - h) / (networkW - networkH - 5/6)
	// <=> nsize = ssize * (1 - networkW) / (networkW + 1)
	int space_size = (w - h) / (networkW - networkH - 5 / 6); // Espace entre les couches
	int neuron_margin = space_size / 6;
	int neuron_size = space_size * (1 - networkW) / (networkW + 1);

	int x = space_size + neuron_size / 2;
	while (ll->next) {
		drawLayer(window, ll->layer, ll->next->layer, x, h/2, neuron_size, neuron_margin);
		x += neuron_size + space_size;
		ll = ll->next;
	}
	drawLayer(window, ll->layer, Layer(0, NULL), x, h/2, neuron_size, neuron_margin);


}

void drawLayer(SDL_Surface* window, Layer l, Layer l_next, int x, int y, int size, int margin) {

	int nb_neurons = l.getSize();
	if (nb_neurons == 0) return;

	float* values = l.getValues();
	float* W = l.getWeights();

	int space = size + margin;

	y -= (nb_neurons - 1) * space / 2; // On positionne le y en haut de la couche.

	for (int i = 0; i < nb_neurons; i++) {
		drawNeuron(window, setPoint(x, y), values[i], size);
		y += space;
	}

}


int getMaxLayerSize(LayerList* ll) {
	int max = 0, tmp;
	while (ll) {
		if ((tmp = ll->layer.getSize()) > max) max = tmp;
		ll = ll->next;
	}
	return max;
}

pointf unit(pointf v) {
	return scale(v, 1.0 / norm(v));
}

pointf scale(pointf v, float f) {
	v.x *= f;
	v.y *= f;
	return v;
}

point sumPoint(point p1, point p2, int diff) {
	if (diff != -1) diff = 1;
	p1.x += p2.x * diff;
	p1.y += p2.y * diff;
	return p1;
}

pointf sumPointf(pointf p1, pointf p2, int diff) {
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

pointf setPointf(float x, float y) {
	pointf p;
	p.x = x;
	p.y = y;
	return p;
}

float norm(pointf v) {
	return sqrtf(v.x * v.x + v.y * v.y);
}