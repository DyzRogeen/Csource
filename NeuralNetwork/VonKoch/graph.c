#include "graph.h"
#define max(a,b) a > b ? a : b
#define min(a,b) a < b ? a : b

void drawNetwork(SDL_Surface* window, NeuralNetwork* n) {
	Uint32* pxls = (Uint32*)window->pixels;
	int w = window->w, h = window->h;

	LayerList* ll = n->layerList;
	int networkW = n->size;
	int networkH = getMaxLayerSize(ll, h / 2);

	// (nsize + ssize) * networkW + 2*nsize - ssize = (networkW + 2) * nsize + (networkW - 1) * ssize = w
	// (nsize + ssize/12) * networkH + nsize - ssize / 6 = (networkH + 1) * nsize + (networkH - 1)/12 * ssize = h
	// <=> a * nsize + b * ssize = w
	// <=> c * nsize + d * ssize = h
	// <=> nsize * D = w *  (networkH - 1)/12 + h * (1 - networkW)
	// <=> ssize * D = w * -(networkH + 1)   + h * (networkW + 1)
//	float det = (networkW + 2) * (networkH - 1) / 12.0 - (networkW - 1) * (networkH + 1);
//	int neuron_size = (w * (networkH - 1) / 12.0 + h * (1 - networkW)) / det;
//	int space_size = (w * -(networkH + 1) + h * (networkW + 2)) / det; // Espace entre les couches
//	int neuron_margin = space_size / 12.0;

	// h = (networkH + 1) * neuron_size + (networkH - 1) * neuron_size / networkH
	// w = networkW * neuron_size + networkW * space_size
	// neuron_size = h / (networkH * 1.1 + 0.9)
	// space_size = (w - (networkW + 2) * neuron_size) / (networkW - 1)

	int neuron_size = min(h / (networkH + 1 + (networkH - 1)/networkH), 100);
	int space_size = (w - (networkW + 0.2) * neuron_size) / (networkW);
	int neuron_margin = round(neuron_size / networkH);

	int x = (space_size + neuron_size) / 2;
	while (ll->next) {
		drawLayer(window, ll->layer, ll->next->layer, x, h / 2, neuron_size, space_size, neuron_margin);
		x += neuron_size + space_size;
		ll = ll->next;
	}
	drawLayer(window, ll->layer, NULL, x, h/2, neuron_size,space_size, neuron_margin);


}

void drawLayer(SDL_Surface* window, layer* l, layer* l_next, int x, int y0, int size, int space, int margin) {

	int nb_neurons = l->size;
	if (nb_neurons == 0 || nb_neurons > window->h / 2) return;

	float* values = l->nodeVals;
	float* W = l->W_mat;

	int nb_neurons_next = l_next == NULL ? 0 : l_next->size;
	int x_next = x + space;
	int y_next;

	int y = y0 - (nb_neurons - 1) * (size + margin) / 2; // On positionne le y en haut de la couche.

	int maxi = 0, cnt;
	for (cnt = 0; cnt < nb_neurons; cnt++) maxi = max(maxi, values[cnt]);
	maxi += cnt;

	for (int i = 0; i < nb_neurons; i++) {
	
		y_next = y0 - (nb_neurons_next - 1) * (size + margin) / 2;
		// Dessins des liens
		for (int j = 0; j < nb_neurons_next; j++) {
			drawLink(window, setPoint(x, y), setPoint(x_next, y_next), size, *W);
			y_next += size + margin;
			W++;
		}

		drawNeuron(window, x, y, values[i] / maxi, size / 2);

		y += size + margin;
	}

}

void drawNeuron(SDL_Surface* window, int px, int py, float val, int size) {
	Uint32* pxls = (Uint32*)window->pixels;
	int w = window->w, h = window->h;
	int width = size < 10 ? size < 5 ? 1 : 2 : 3;
	int margin_size = size / 5;
	margin_size += width;
	int size2 = size * size, size2_in = pow(size - width, 2), size2_margin = pow(size - margin_size, 2);
	int hsize, hsize_in, hsize_margin, yslotP, yslotM;

	Uint8 tmp = max(0, val) * 255;
	Uint32 color, n_color = (tmp << 16) | (tmp << 8) | tmp;

	for (int y = 0; y <= size; y++) {
		yslotP = (py + y) * w;
		yslotM = (py - y) * w;

		hsize = round(sqrt(size2 - y * y));
		hsize_margin = (size - margin_size < y) ? 0 : round(sqrt(size2_margin - y * y));
		hsize_in = (size - width < y) ? 0 : round(sqrt(size2_in - y * y));

		for (int x = 0; x <= hsize; x++) {
			color = x >= hsize_margin ? (x >= hsize_in ? WHITE : 0) : n_color; // Blanc si contours et Noir si dans la marge
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

	pointf v = setPointf(p2.x - p1.x + 4, p2.y - p1.y);
	float l = norm(v);
	v = scale(v, 1.0 / l);

	w *= 2;
	Uint32 color = w > 1 ? BLUE : w < -1 ? RED : w > 0 ? (Uint32)(w * 255) : (Uint32)(-w * 255) << 16;

	float x = p1.x + size / 2 - 2, y = p1.y;
	for (int i = 0; i < l; i++) {
		*(pxls + (int)x + (int)y * width) = color;
		x += v.x;
		y += v.y;
	}

}

int getMaxLayerSize(LayerList* ll, int maxSize) {
	int max = 0, tmp;
	while (ll) {
		if ((tmp = ll->layer->size) > max && tmp < maxSize) max = tmp;
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