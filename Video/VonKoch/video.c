#include "video.h"

void mapPxls(SDL_Surface* window, SDL_Surface* img) { // TODO params de w et h manuellement
	int w = window->w, h = window->h;
	Uint32* pxls = window->pixels, * img_pxls = img->pixels;

	for (int x = 0; x < w; x++)
		for (int y = 0; y < h; y++)
			*(pxls + x + y * w) = *(img_pxls + x + y * w);

}

int* getGaussianDiffFilter(int dim_filter, int verOrHor) {

	float sigma = 0.04 * dim_filter, const1 = 1.f / (sigma * sigma * 2), const2 = const1 / PI;

	float* filter = (float*)calloc(dim_filter * dim_filter, sizeof(float));

	int center = (dim_filter - 1) / 2;

	for (float x = -center; x <= center; x++)
		for (float y = -center; y <= center; y++)
			filter[(int)((x + center) + (y + center) * dim_filter)] = (verOrHor ? x : y) * const2 * pow(E, -(x*x + y*y) / const1);


	return filter;

}

void convolution(SDL_Surface* win, float* filter, int w_filter, int h_filter) {
	blackWhite(win);

	int w = win->w, h = win->h, win_size = w * h, slot_win = 0, slot_filter, val_pxl;
	Uint32* pxls = win->pixels, *pxls_copy = copyPxls(win);
	Uint8* pxl;

	int side_x = (w_filter - 1) / 2, side_y = (h_filter - 1) / 2;

	for (int y = 0; y < h; y++)
		for (int x = 0; x < w; x++) {

			val_pxl = slot_filter = 0;
			for (int j = -side_y; j <= side_y; j++)
				for (int i = -side_x; i <= side_x; i++) {

					if (x + i < 0 || x + i >= w || y + j < 0 || y + j >= h) {
						slot_filter++;
						continue;
					}

					val_pxl += (*(pxls_copy + x + i + (y + j) * w) & 255) * filter[slot_filter++];
				}

			val_pxl += 128;
			val_pxl = val_pxl < 0 ? 0 : (val_pxl > 255 ? 255 : val_pxl);

			pxl = pxls + slot_win;
			pxl[0] = pxl[1] = pxl[2] = val_pxl;

			slot_win++;

		}

	free(pxls_copy);
}

void gaussianNoiseReducer(SDL_Surface* win) {
	/*int filter[25] = {
		2, 4, 5, 4, 2,
		4, 9,12, 9, 4,
		5,12,15,12, 5,
		4, 9,12, 9, 4,
		2, 4, 5, 4, 2,
	};*/

	int filter[25] = {
		1, 4, 6, 4, 1,
		4,16,24,16, 4,
		6,24,36,24, 6,
		4,16,24,16, 4,
		1, 4, 6, 4, 1,
	};

	int w = win->w, h = win->h, slot = 0, val_pxl, slot_filter, slot_win = 0;
	Uint32* pxls = win->pixels, * pxls_copy = copyPxls(win);
	Uint8* pxl;

	for (int y = 0; y < h; y++)
		for (int x = 0; x < w; x++) {
			
			val_pxl = slot_filter = 0;
			for (int j = -2; j <= 2; j++)
				for (int i = -2; i <= 2; i++) {

					if (x + i < 0 || x + i >= w || y + j < 0 || y + j >= h) {
						slot_filter++;
						continue;
					}

					val_pxl += (*(pxls_copy + x + i + (y + j) * w) & 255) * filter[slot_filter];
				}

			val_pxl /= 25;
			val_pxl = val_pxl > 60 ? val_pxl : 0;

			pxl = pxls + slot_win;
			pxl[0] = pxl[1] = pxl[2] = val_pxl;

			slot_win++;

		}

	free(pxls_copy);

}

void normIMGs(SDL_Surface* win, Uint32* pxlsH, Uint32* pxlsV) { // TODO Filtre de Canny
	int w = win->w, h = win->h, slot = 0, valPxl, pxlH, pxlV;
	float sq2 = 1.f / sqrt(2);
	Uint32* pxls = win->pixels;
	Uint8* pxl;

	for (int x = 0; x < w; x++)
		for (int y = 0; y < h; y++) {
			pxl = pxls + slot;
			pxlH = *(pxlsH + slot) & 255 - 128;
			pxlV = *(pxlsV + slot) & 255 - 128;

			valPxl = (valPxl = sqrt(pxlH * pxlH + pxlV * pxlV) * sq2) > 80 ? valPxl : valPxl;

			pxl[0] = pxl[1] = pxl[2] = valPxl;

			slot++;
		}

			
}

void blackWhite(SDL_Surface* win) {
	int w = win->w, h = win->h;
	Uint32* pxls = win->pixels;

	Uint8* pxl;
	Uint32 valBW;
	for (int x = 0; x < w; x++)
		for (int y = 0; y < h; y++) {
			pxl = pxls + x + y * w;
			valBW = (pxl[0] + pxl[1] + pxl[2]) / 3;
			pxl[0] = pxl[1] = pxl[2] = valBW;
		}
}

void detectCorners(SDL_Surface* win) {
	int w = win->w, h = win->h, slot = 0;
	Uint32* pxls = win->pixels;
	int pxl, pxl_m1, pxl_bis;
	int disparityFound = 0;

	int areaDetection = 7;
	int deltaMax = 50;
	int side = (areaDetection - 1) / 2;

	int seuil = 20;
	float diag_side = (float)side / sqrt(2);
	int nb_brighter, nb_darker, streak_bright, streak_dark;

	for (int x = side + 1; x < w - side; x++)
		for (int y = side + 1; y < h - side; y++) {

			nb_brighter = nb_darker = streak_bright = streak_dark = 0;
			slot = x + y * w;
			pxl = *(pxls + slot) & 255;

			pxl_bis = *(pxls + slot - side) & 255;
			if (pxl - pxl_bis < -seuil) {
				nb_brighter++;
				streak_bright++;
				streak_dark = 0;
			}
			else if (pxl - pxl_bis > seuil) nb_darker++;

			pxl_bis = *(pxls + slot + side) & 255;
			if (pxl - pxl_bis < -seuil) nb_brighter++;
			else if (pxl - pxl_bis > seuil) nb_darker++;

			pxl_bis = *(pxls + slot - side * w) & 255;
			if (pxl - pxl_bis < -seuil) nb_brighter++;
			else if (pxl - pxl_bis > seuil) nb_darker++;

			pxl_bis = *(pxls + slot + side * w) & 255;
			if (pxl - pxl_bis < -seuil) nb_brighter++;
			else if (pxl - pxl_bis > seuil) nb_darker++;

			pxl_bis = *(pxls + (int)(slot - diag_side - diag_side * w)) & 255;
			if (pxl - pxl_bis < -seuil) nb_brighter++;
			else if (pxl - pxl_bis > seuil) nb_darker++;

			pxl_bis = *(pxls + (int)(slot - diag_side + diag_side * w)) & 255;
			if (pxl - pxl_bis < -seuil) nb_brighter++;
			else if (pxl - pxl_bis > seuil) nb_darker++;

			pxl_bis = *(pxls + (int)(slot + diag_side - diag_side * w)) & 255;
			if (pxl - pxl_bis < -seuil) nb_brighter++;
			else if (pxl - pxl_bis > seuil) nb_darker++;

			pxl_bis = *(pxls + (int)(slot + diag_side + diag_side * w)) & 255;
			if (pxl - pxl_bis < -seuil) nb_brighter++;
			else if (pxl - pxl_bis > seuil) nb_darker++;

			// TODO regarder si 2 à 3 ou 6 points brigter ou darker sont consécutifs

			if (nb_brighter > 2 )
				*(pxls + x + y * w) |= 255 << 16;

			continue;

			/* Vertical changes
				
				(exemple avec zone de rechereche 5x5)
				=========== Disparités sur ligne 1 ?
				 . . . . .
				=========== Sinon sur ligne 2 ?
				 . . . . .
				=========== Sinon sur ligne 3 ?
			*/
			disparityFound = 0;
			for (int n = -1; n <= 1 && !disparityFound; n++) { // On observe 3 lignes, les 2 bords et la ligne du centre

				slot = x - side + (y - side * n) * w;
				for (int i = 0; i < areaDetection; i++) {
					pxl = *(pxls + slot) & 255;

					// S'il y a du changement, on inspecte la direction suivante
					if (i != 0 && abs(pxl_m1 - pxl) > deltaMax) {
						disparityFound = 1;
						break;
					}

					pxl_m1 = pxl;
					slot++;
				}

			}
			

			if (!disparityFound) continue; // Si la zone est "lisse" dans une direction, on admet que ca ne peut pas être un coin.


			/* Horizontal changes
			* 
			* On revérifie les disparités mais cette fois sur 3 colonnes de la zone de recherche.
			*   c1   c2   c3
				|| . || . ||
				|| . || . ||
				|| . || . ||
				|| . || . ||
				|| . || . ||
			*/
			disparityFound = 0;
			for (int n = -1; n <= 1 && !disparityFound; n++) { // On observe 3 colones, les 2 bords et la colones du centre.

				slot = x - side * n + (y - side) * w;
				for (int i = 0; i < areaDetection; i++) {
					pxl = *(pxls + slot) & 255;

					// S'il y a du changement, on inspecte la direction suivante
					if (i != 0 && abs(pxl_m1 - pxl) > deltaMax) {
						disparityFound = 1;
						break;
					}

					pxl_m1 = pxl;
					slot += w;
				}

			}

			if (disparityFound) { // S'il y a du changement dans les 2 directions, on peut admettre que c'est un coin.
				*(pxls + x + y * w) |= 255 << 16; // On le colorie en rouge.
			}

		}

}


// UTILS

Uint32* copyPxls(SDL_Surface* win) {
	int w = win->w, h = win->h;
	Uint32* pxls = win->pixels;

	Uint32* pxls_copy = (Uint32*)calloc(w * h, 4);
	for (int i = 0; i < w * h; i++) *(pxls_copy + i) = *(pxls + i);

	return pxls_copy;
}

float getFilterWeight(float* filter, int w_filter, int h_filter) {

	float sum = 0.f;
	for (int y = 0; y < h_filter; y++) {
		for (int x = 0; x < w_filter; x++)
			sum += filter[(int)(x + y * w_filter)];
	}
	return sum;

}

void printFilter(float* filter, int w_filter, int h_filter) {

	for (int y = 0; y < h_filter; y++) {
		printf("[  ");
		for (int x = 0; x < w_filter; x++)
			printf("%.2f  ", filter[(int)(x + y * w_filter)]);
		printf("]\n");
	}
	printf("\n");

}
