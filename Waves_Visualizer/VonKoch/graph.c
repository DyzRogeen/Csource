#include "graph.h"

void update(float* tab, int w, int h) {

	float* tab_copy = (float*)calloc(w * h, sizeof(float));

	for (int i = 0; i < w * h; i++) tab_copy[i] = tab[i];

	float sum, alpha = 0.6;

	int slot;
	for (int y = 1; y < h - 1; y++) {

		slot = y * w + 1;
		for (int x = 1; x < w - 1; x++) {

			sum = 0;
			// On calcule la moyenne du voisinage du pixel
			//for (int j = -1; j <= 1; j++) {
			//	for (int i = -1; i <= 1; i++)
			//		if (i || j) sum += tab_copy[slot + i + j * w] * (i && j ? INV_SQRT2 : 1);
			//}
			

			if (tab[slot] > 0) {
//				printf("");
			}

			//tab[slot] += (sum / CUMUL - tab[slot]) * alpha;
			tab[slot - 1] += tab_copy[slot] * alpha;
			tab[slot + 1] += tab_copy[slot] * alpha;
			tab[slot] -= 2 * tab_copy[slot] * alpha * 1.2;

			tab[slot - 1] = max(min(tab[slot - 1], 1), -1);
			tab[slot + 1] = max(min(tab[slot + 1], 1), -1);
			tab[slot] = max(min(tab[slot], 1), -1);

			slot++;

		}

	}

	free(tab_copy);

}

void colorWindow(SDL_Surface* s, float* tab) {
	int w = s->w, h = s->h;
	Uint32* pxls = s->pixels, * pxls_copy = NULL;

	//for (int i = 0; i < w * h; i++) *(pxls++) = getColor(*(tab++));

	tab += w;
	for (int i = 0; i < w; i++) {

		Uint32 c = getColor(*(tab++));
		for (int j = 0; j < h; j++)
			pxls[i + j * w] = c;
	}
}

Uint32 getColor(float val) {
	Uint8 pxl_val = (val + 1) * 127;
	return 0xFF000000 | pxl_val << 16 | pxl_val << 8 | pxl_val;
}