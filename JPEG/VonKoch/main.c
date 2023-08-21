#define _CRT_SECURE_NO_WARNINGS 0
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include "utils_SDL.h"
#include "huff.h"

#include "../SDL2_image-2.6.3/include/SDL_image.h"

#define PI	3.14159265359

typedef struct sPoint{
	int x;
	int y;
}point;

Uint32* cpyIMG(SDL_Surface* img) {

	int w = img->w;
	int h = img->h;

	Uint32* cpy = (Uint32*)calloc(w * h, sizeof(Uint32));
	Uint32* pxl = (Uint32*)(img->pixels);

	for (int x = 0; x < w; x++)
		for (int y = 0; y < h; y++) *(cpy + x + y * w) = *(pxl + x + y * w);

	return cpy;

}

Uint32* getPxW(SDL_Surface* window, int x, int y) {

	return (Uint32*)(window->pixels) + x + y * window->w;

}

Uint32 getPixel(SDL_Surface* surface, int x, int y, int bpp) {

	Uint8* p = (Uint8*)(surface->pixels) + y * surface->pitch + x * bpp;

	switch (bpp) {
	case 1:
		return *p;

	case 2:
		return *(Uint16*)p;

	case 3:
		if (SDL_BYTEORDER == SDL_BIG_ENDIAN)
			return p[0] << 16 | p[1] << 8 | p[2];
		else
			return p[0] | p[1] << 8 | p[2] << 16;

	case 4:
		return p[0] << 16 | p[1] << 8 | p[2] | 100 << 24;

	default:
		return 0; /* shouldn't happen, but avoids warnings */
	}
}

void setMatrix(SDL_Surface* window, SDL_Surface* img) {

	int bpp = img->format->BytesPerPixel;
	int w = window->w;
	int h = window->h;

	printf("Processing...\n");
	for (int x = 0; x < w; x++)
		for (int y = 0; y < h; y++) *((Uint32*)(window->pixels) + x + y * window->w) = getPixel(img, x, y, 4);

	SDL_UpdateRect(window, 0, 0, 0, 0);
	SDL_Flip(window);

	printf("Done !\n");
	
}

void pixelize(SDL_Surface* window, int deg) {

	int deg2 = deg * deg;
	int w = window->w - deg;
	int h = window->h - deg;

	int PxSum[3];
	Uint8 PxSum8[4];
	Uint8* p;

	for (int x = 0; x < w; x += deg) {
		for (int y = 0; y < h; y += deg) {

			PxSum[0] = 0;
			PxSum[1] = 0;
			PxSum[2] = 0;

			for (int i = 0; i < deg; i++)
				for (int j = 0; j < deg; j++) {
					p = (Uint8*)getPxW(window, x + i, y + j);
					PxSum[0] += p[0];
					PxSum[1] += p[1];
					PxSum[2] += p[2];
				}

			PxSum8[0] = PxSum[0] / deg2;
			PxSum8[1] = PxSum[1] / deg2;
			PxSum8[2] = PxSum[2] / deg2;

			for (int i = 0; i < deg; i++)
				for (int j = 0; j < deg; j++) *getPxW(window, x + i, y + j) = *(Uint32*)PxSum8;

		}
	}

	SDL_UpdateRect(window, 0, 0, 0, 0);
	SDL_Flip(window);

}

void downSample(SDL_Surface* window, Uint8* img, int deg) {

	int w = window->w;
	int h = window->h;
	int x, y, i, j;
	int x_rest = deg, y_rest = deg;
	float sum;

	for (x = 0; x < w; x += deg) {
		for (y = 0; y < h; y += deg) {

			if (x + deg > w) x_rest = w - x;
			if (y + deg > h) y_rest = h - y;

			sum = 0;
			for (i = 0; i < x_rest; i++)
				for (j = 0; j < y_rest; j++) sum += *(img + (x + i) + (y + j) * w);

			sum = roundf(sum / (x_rest * y_rest));

			for (i = 0; i < x_rest; i++)
				for (j = 0; j < y_rest; j++) *(img + (x + i) + (y + j) * w) = sum;
		}
		y_rest = deg;
	}

}

void smooth(SDL_Surface* window, float pct) {

	float pct2 = pct * pct;
	Uint32* cpy = cpyIMG(window);
	Uint32* pxl = (Uint32*)(window->pixels);

	Uint8* p, *pij;

	int w = window->w;
	int h = window->h;

	for (int x = 0; x < w; x++) {
		for (int y = 0; y < h; y++) {

			for (int i = x - 1; i <= x + 1; i++) {

				if (i < 0 || i > w) continue;

				for (int j = y - 1; j <= y + 1; j++) {

					if (j < 0 || j > h || (i == x && j == y)) continue;

					p = (Uint8*)(pxl + x + y * w);
					pij = (Uint8*)(pxl + i + j * w);

					// p = (p + pij * pct) / (1 + pct)
					if (i == x || j == y) {
						p[0] = (Uint8)((p[0] + pij[0] * pct) / (1 + pct));
						p[1] = (Uint8)((p[1] + pij[1] * pct) / (1 + pct));
						p[2] = (Uint8)((p[2] + pij[2] * pct) / (1 + pct));
					}
					else {
						p[0] = (Uint8)((p[0] + pij[0] * pct2) / (1 + pct2));
						p[1] = (Uint8)((p[1] + pij[1] * pct2) / (1 + pct2));
						p[2] = (Uint8)((p[2] + pij[2] * pct2) / (1 + pct2));
					}

				}
			}

		}
	}

	free(cpy);

	SDL_UpdateRect(window, 0, 0, 0, 0);
	SDL_Flip(window);

}

void homogene(SDL_Surface* window, float pct) {

	float pct2 = pct * pct;
	Uint32* cpy = cpyIMG(window);
	Uint32* pxl = (Uint32*)(window->pixels);

	Uint8* p = NULL, * pij;
	Uint32* tab[8];

	int w = window->w;
	int h = window->h;

	int nP;
	int PxSum[3];
	Uint8 PxSum8[4];

	for (int x = 0; x < w; x++) {
		for (int y = 0; y < h; y++) {

			nP = 0;

			for (int i = x - 1; i <= x + 1; i++) {

				if (i < 0 || i > w) continue;

				for (int j = y - 1; j <= y + 1; j++) {

					if (j < 0 || j > h || (i == x && j == y)) continue;

					p = (Uint8*)(pxl + x + y * w);
					pij = (Uint8*)(pxl + i + j * w);

					// p = (p + pij * pct) / (1 + pct)
					if (p[0] < (1 + pct) * pij[0] && p[0] > (1 - pct) * pij[0] && p[1] < (1 + pct) * pij[1] && p[1] > (1 - pct) * pij[1] && p[2] < (1 + pct) * pij[2] && p[2] > (1 - pct) * pij[2]) {
						tab[nP] = pxl + i + j * w;
						nP++;
					}
				}
			}

			if (nP) {

				PxSum[0] = p[0];
				PxSum[1] = p[1];
				PxSum[2] = p[2];

				for (int ind = 0; ind < nP; ind++) {
					pij = (Uint8*)(tab[ind]);
					PxSum[0] += pij[0];
					PxSum[1] += pij[1];
					PxSum[2] += pij[2];
				}

				PxSum8[0] = PxSum[0] / (nP + 1);
				PxSum8[1] = PxSum[1] / (nP + 1);
				PxSum8[2] = PxSum[2] / (nP + 1);

				p = *(Uint32*)PxSum8;
				for (int ind = 0; ind < nP; ind++) *(tab[ind]) = p;
			}

		}
	}

	free(cpy);

	SDL_UpdateRect(window, 0, 0, 0, 0);
	SDL_Flip(window);

}

void negative(SDL_Surface* window) {

	int w = window->w;
	int h = window->h;
	Uint32* pxl = (Uint32*)(window->pixels);
	Uint8* p;

	for (int x = 0; x < w; x++)
		for (int y = 0; y < h; y++) {

			p = (Uint8*)(pxl + x + y * w);
			p[0] = 255 - p[0];
			p[1] = 255 - p[1];
			p[2] = 255 - p[2];

		}

	SDL_UpdateRect(window, 0, 0, 0, 0);
	SDL_Flip(window);

}

Uint8* blackWhite(SDL_Surface* window, int comp) {

	int w = window->w;
	int h = window->h;
	
	Uint32* pxl;
	Uint8* bw = NULL;
	Uint8* p;

	if (comp) {
		pxl = cpyIMG(window);
		bw = (Uint8*)calloc(w * h, sizeof(Uint8));
	}
	else pxl = (Uint32*)(window->pixels);

	Uint8 sum;

	for (int x = 0; x < w; x++)
		for (int y = 0; y < h; y++) {

			sum = 0;

			p = (Uint8*)(pxl + x + y * w);
			sum = roundf(((int)(p[0]) + (int)(p[1]) + (int)(p[2])) / 3);
			if (comp) {
				bw[x + y * w] = sum;
				continue;
			}
			p[0] = sum;
			p[1] = sum;
			p[2] = sum;

		}

	if (comp) {
		free(pxl);
		return bw;
	}

	SDL_UpdateRect(window, 0, 0, 0, 0);
	SDL_Flip(window);

}

Uint8* chrominance(SDL_Surface* window, Uint8* bw, int mode) {

	int w = window->w;
	int h = window->h;

	Uint8* chrom = NULL;
	if (!mode) bw = blackWhite(window, 1);
	else chrom = (Uint8*)calloc(w * h, sizeof(Uint8));

	Uint32* pxl = (Uint32*)(window->pixels);
	Uint8* p;
	Uint8 valBW;

	int x, y, ind;

	for (x = 0; x < w; x++)
		for (y = 0; y < h; y++) {

			ind = x + y * w;
			p = (Uint8*)(pxl + ind);
			valBW = *(bw + ind);

			if (mode == 0) {
				p[0] = (p[0] - valBW) * 0.75 + 128;
				p[1] = 0;
				p[2] = (p[2] - valBW) * 0.75 + 128;
			}
			else chrom[ind] = roundf((p[(mode == 1 ? 0 : 2)] - valBW) * 0.75) + 128;

		}

	if (mode) return chrom;

	SDL_UpdateRect(window, 0, 0, 0, 0);
	SDL_Flip(window);

};

int getCoefs(float* f, float* a, float* b, int nbCoefs, int x_rest, int y_rest) {

	int area2 = x_rest * y_rest;
	float ratio = 2 * PI / area2;

	int coefs_cnt = 0;

	for (int n = 1; n < nbCoefs; n++) {
		a[n] = 0;
		b[n] = 0;
		for (int x = 0; x < area2; x++) {

			a[n] += f[x] * cos(ratio * x * n);
			b[n] += f[x] * sin(ratio * x * n);
		}
		a[n] *= 2 / (float)area2;
		b[n] *= 2 / (float)area2;
		if ((int)a[n] != 0 || (int)b[n] != 0) coefs_cnt++;
			//printf("%.1fcos(%dx) + %.1fisin(%dx)\n", a[n], n, b[n], n);
	}
	//printf("\n");
	return coefs_cnt;
}

void getFunc(float* f, float* a, float* b, int nbCoefs, int x_rest, int y_rest) {


	int area2 = x_rest * y_rest;
	float ratio = 2 * PI / area2;

	for (int x = 0; x < area2; x++) f[x] = 128;

	for (int n = 1; n < nbCoefs; n++)
		for (int x = 0; x < area2; x++)
			f[x] += a[n] * cos(ratio * x * n) + b[n] * sin(ratio * x * n);

	//for (int x = 0; x < area2; x++) printf("f[%d] = %d\n", x, f[x]);
	for (int x = 0; x < area2; x++) {
		if (f[x] < 0) f[x] = 0;
		else if (f[x] > 255) f[x] = 255;
	}

}

void fourierComp(SDL_Surface* window, int nbCoefs, int area) {

	int area2 = area * area;
	float* coef_a = (float*)calloc(nbCoefs, sizeof(float));
	float* coef_b = (float*)calloc(nbCoefs, sizeof(float));

	float* fR = (float*)calloc(area2, sizeof(float));
	float* fG = (float*)calloc(area2, sizeof(float));
	float* fB = (float*)calloc(area2, sizeof(float));

	int w = window->w;
	int h = window->h;
	int x, y;
	int x_rest, y_rest;

	int prog = 0;
	float prog0 = 0;

	int coefs_cnt = 0;

	Uint32* pxl = (Uint32*)(window->pixels);
	Uint8* p;

	for (int x_step = 0; x_step < w; x_step += area)
		for (int y_step = 0; y_step < h; y_step += area) {

			if (x_step + area >= w) x_rest = w - x_step;
			else x_rest = area;

			if (y_step + area >= h) y_rest = h - y_step;
			else y_rest = area;

			for (x = 0; x < x_rest; x++)
				for (y = 0; y < y_rest; y++) {
					p = (Uint8*)(pxl + x_step + x + (y_step + y) * w);
					fR[x + y * x_rest] = p[0] - 128;
					fG[x + y * x_rest] = p[1] - 128;
					fB[x + y * x_rest] = p[2] - 128;
				}

			//if (x_step == 0 && y_step == 0)
			coefs_cnt += getCoefs(fR, coef_a, coef_b, nbCoefs, x_rest, y_rest);
			getFunc(fR, coef_a, coef_b, nbCoefs, x_rest, y_rest);

			coefs_cnt += getCoefs(fG, coef_a, coef_b, nbCoefs, x_rest, y_rest);
			getFunc(fG, coef_a, coef_b, nbCoefs, x_rest, y_rest);

			coefs_cnt += getCoefs(fB, coef_a, coef_b, nbCoefs, x_rest, y_rest);
			getFunc(fB, coef_a, coef_b, nbCoefs, x_rest, y_rest);

			for (x = 0; x < x_rest; x++)
				for (y = 0; y < y_rest; y++) {

					p = (Uint8*)(pxl + x_step + x + (y_step + y) * w);
					p[0] = (Uint8)(fR[x + y * x_rest]);
					p[1] = (Uint8)(fG[x + y * x_rest]);
					p[2] = (Uint8)(fB[x + y * x_rest]);

				}

			if ((int)(prog0 += 100 * x_rest * y_rest / (float)(w * h)) > prog) {
				prog = prog0;
				printf("Fourier Compression Completed at %d %%\n", prog);
			}
			
		}

	free(fR);
	free(fG);
	free(fB);
	free(coef_a);
	free(coef_b);

	SDL_UpdateRect(window, 0, 0, 0, 0);
	SDL_Flip(window);

	printf("Done!\nCompression %d Bytes => %d Bytes (%d %%)\n", w * h * 3, coefs_cnt * 8, 100 - 100 * coefs_cnt * 8 / (w * h * 3));

}

Uint8* DCT(SDL_Surface* window, Uint8* YCbCr, int sample, int lim, float rangeRate, Uint8* Q, int showRes) {

	int w = window->w;
	int h = window->h;
	int x, y, x_step, y_step, u, v;
	int sampleUV = 8;

	float* vals = (float*)calloc(w * h, sizeof(float));

	float sumG;
	float ratioU, ratioV;
	float sqr2 = 1 / sqrtf(2);
	Uint8 G;
	Uint8* Gtab = (Uint8*)calloc(w * h, sizeof(Uint8));
	int Gind = 0;
	int tmpG;

	int ind;
	int prog = 0;
	float prog0 = 0;
	int rangeRateUp = 128 * (1 + rangeRate), rangeRateDown = 128 * (1 - rangeRate);

	for (x_step = 0; x_step < w; x_step += sample)
		for (y_step = 0; y_step < h; y_step += sample) {

			if (x_step + sample > w) x_step = w - sample + 1;
			if (y_step + sample > h) y_step = h - sample + 1;

			for (u = 0; u < sampleUV; u++) {
				for (v = 0; v < sampleUV; v++) {

					// Ignore higher frequencies
					if (u + v >= lim) continue;

					sumG = 0;
					ratioU = u * PI * 0.0625;
					ratioV = v * PI * 0.0625;

					for (x = 0; x < sample; x++)
						for (y = 0; y < sample; y++) {
							ind = (x + x_step) + (y + y_step) * w;
							sumG += (YCbCr[ind] - 128) * cos((2 * x + 1) * ratioU) * cos((2 * y + 1) * ratioV);
							if (u == 0 && v == 0) vals[ind] = 128;
						}
					
					// Setting G value and Quantization
					Gtab[Gind++] = G = ((tmpG = roundf((sumG * 0.5 * (u == 0 ? sqr2 : 1) * (v == 0 ? sqr2 : 1)) / *(Q + u + v * sampleUV)) + 128) < 1 ? 1 : (tmpG > 255 ? 255 : tmpG));
					if (!showRes && G <= rangeRateUp && G >= rangeRateDown) continue;

					// Reversing
					for (x = 0; x < sample; x++)
						for (y = 0; y < sample; y++) {
							ind = (x + x_step) + (y + y_step) * w;

							vals[ind] += (G - 128) * cos((2 * x + 1) * ratioU) * cos((2 * y + 1) * ratioV);
						}
					
				}
			}

			if ((int)(prog0 += 100 * sample * sample / (float)(w * h)) - 1 > prog) {
				prog = prog0;
				//printf("DCT Completed at %d %%\n", prog);
			}

		}

	if (showRes) {
		for (x = 0; x < w; x++)
			for (y = 0; y < h; y++) {
				ind = x + y * w;
				YCbCr[ind] = (Uint8)(vals[ind] < 0 ? 0 : (vals[ind] > 255 ? 255 : vals[ind]));
			}
	}
	
	free(vals);
	return Gtab;
}

void assemble(SDL_Surface* window, Uint8* Y, Uint8* Cr, Uint8* Cb, float dSample) {

	int w = window->w;
	int h = window->h;

	Uint8* chrom = NULL;

	Uint32* pxl = (Uint32*)(window->pixels);
	Uint8* p;
	int pY, pCr, pCb, pf;

	int x, y, ind;

	for (x = 0; x < w; x++)
		for (y = 0; y < h; y++) {

			ind = x + y * w;
			p = (Uint8*)(pxl + ind);
			pY = *(Y + ind);
			pCr = *(Cr + (int)(x / dSample + (int)(y / dSample) * roundf(w / dSample)));
			pCb = *(Cb + (int)(x / dSample + (int)(y / dSample) * roundf(w / dSample)));
			//if (x < 3)printf("(%d, %d) => %c%c [%d, %d]\n", x, y, pCr, pCb, (int)(x / dSample), (int)(y / 16) * 21);

			p[0] = ((pf = pY + roundf((pCr - 128) * 4 / 3)) > 255 ? 255 : (pf < 0 ? 0 : pf));
			p[1] = ((pf = pY - roundf((pCr + pCb - 256) * 4 / 3)) > 255 ? 255 : (pf < 0 ? 0 : pf));
			p[2] = ((pf = pY + roundf((pCb- 128) * 4 / 3)) > 255 ? 255 : (pf < 0 ? 0 : pf));

		}

	SDL_UpdateRect(window, 0, 0, 0, 0);
	SDL_Flip(window);
}

Uint8* invDCT(SDL_Surface* window, Uint8* G, int lim, int rangeRate) {

	Uint8 Gval;
	int x_step, y_step, u, v, x, y, ind, sampleUV = 8;
	float ratioU, ratioV;

	int w = window->w;
	int h = window->h;
	float* vals = (float*)calloc(w * h, sizeof(float));
	Uint8* Y = (Uint8*)calloc(w * h, sizeof(Uint8));
	int rangeRateUp = 128 * (1 + rangeRate), rangeRateDown = 128 * (1 - rangeRate);
	int Gind = 0;

	for (x_step = 0; x_step < w; x_step += sampleUV)
		for (y_step = 0; y_step < h; y_step += sampleUV) {

			if (x_step + sampleUV > w) x_step = w - sampleUV + 1;
			if (y_step + sampleUV > h) y_step = h - sampleUV + 1;

			for (u = 0; u < sampleUV; u++) {
				for (v = 0; v < sampleUV; v++) {

					// Ignore higher frequencies
					if (u + v >= lim) continue;

					ratioU = u * PI * 0.0625;
					ratioV = v * PI * 0.0625;

					Gval = G[Gind++];
					// Setting G value and Quantization
					if (Gval < rangeRateUp && Gval > rangeRateDown) continue;

					// Reversing
					for (x = 0; x < sampleUV; x++)
						for (y = 0; y < sampleUV; y++) {
							ind = (x + x_step) + (y + y_step) * w;

							vals[ind] += (Gval - 128) * cos((2 * x + 1) * ratioU) * cos((2 * y + 1) * ratioV);
							//if (x_step == 0 && y_step == 32 * 8) printf("%.0f ", vals[ind] + 128);
						}
					//if (x_step == 0 && y_step == 32 * 8)printf("\n");
				}
			}

		}

	for (x = 0; x < w; x++)
		for (y = 0; y < h; y++) {
			ind = x + y * w;
			vals[ind] += 128;
			Y[ind] = (Uint8)(vals[ind] < 0 ? 0 : (vals[ind] > 255 ? 255 : vals[ind]));
		}

	free(vals);
	return Y;
}

void JPEGDecomp(SDL_Surface* window) {

	int w, h, dSample, limFreq;

	FILE* f = fopen("compressed2.txt", "rb");
	fscanf(f, "%dx%d %d %d\n", &w, &h, &dSample, &limFreq);
	printf("%dx%d %d %d\n", w, h, dSample, limFreq);
	
	Uint8* Cr = (Uint8*)calloc(w * h / dSample ,sizeof(Uint8));
	Uint8* Cb = (Uint8*)calloc(w * h / dSample ,sizeof(Uint8));
	Uint8* G = (Uint8*)calloc(w * h ,sizeof(Uint8));

	char c;
	int ind = 0, x, y;
	int limX = roundf(0.49 + w / (float)dSample), limY = roundf(0.49 + h / (float)dSample);
	printf("%d %.1f %d %.1f\n", limX, w / (float)dSample, limY, h / (float)dSample);
	for (x = 0; x < limX; x++)
		for (y = 0; y < limY; y++) {
			Cr[x + y * limX] = getc(f);
			Cb[x + y * limX] = getc(f);
		}

	c = getc(f);
	ind = 0;
	while (c != '\0') {
		G[ind++] = c;
		if (c == '\r') { if ((c = getc(f)) == '\n') ind--; }
		else c = getc(f);
	}

	fclose(f);

	Uint8* Y = invDCT(window, G, limFreq, 0.02);
	assemble(window, Y, Cr, Cb, dSample);

	SDL_UpdateRect(window, 0, 0, 0, 0);
	SDL_Flip(window);

	free(Cr);
	free(Cb);
	free(G);
	free(Y);

	printf("Decompression Done !\n");

}

void JPEGComp(SDL_Surface* window, Uint8* Q, int dSample, int limFreq, float range, int showRes) {

	// Luminance
	Uint8* Y = blackWhite(window, 1);
	// Red and Blue Chrominances
	Uint8* Cr = chrominance(window, Y, 1);
	Uint8* Cb = chrominance(window, Y, 2);

	// DownSampling Chrominances
	downSample(window, Cr, dSample);
	downSample(window, Cb, dSample);

	// Discrete Cosine Transform (Discrete Fourier Transform with Cosines)
	Uint8* G = DCT(window, Y, 8, limFreq, range, Q, showRes);

	if (showRes) {
		assemble(window, Y, Cr, Cb, 1);

		SDL_UpdateRect(window, 0, 0, 0, 0);
		SDL_Flip(window);
	}

	FILE* f = fopen("compressed.txt", "w+");

	int w = window->w;
	int h = window->h;
	fprintf(f, "%dx%d %d %d\n", w, h, dSample, limFreq);

	int x, y;
	for (x = 0; x < w; x += dSample)
		for (y = 0; y < h; y += dSample) {
			putc(Cr[x + y * w], f);
			putc(Cb[x + y * w], f);
		}
	x = -1;
	while (G[++x]) putc(G[x], f);
	putc('\0', f);

	fclose(f);

	free(Y);
	free(Cr);
	free(Cb);
	free(G);

	huffComp();

	printf("Compression Done !\n");

}

int main(int argc, char **argv)
{

	SDL_Surface * window;
	SDL_Surface * img;
	int quit = 0;
	SDL_Event e;

	Uint8 YCbCr[64] = {
		52, 55, 61, 66, 70, 61, 64, 73,
		63, 59, 55, 90, 109, 85, 69, 72,
		62, 59, 68, 113, 144, 104, 66, 73,
		63, 58, 71, 122, 154, 106, 70, 69,
		67, 61, 68, 104, 126, 88, 68, 70,
		79, 65, 60, 70, 77, 68, 58, 75,
		85, 71, 64, 59, 55, 61, 65, 83,
		87, 79, 69, 68, 65, 76, 78, 94
	};

	Uint8 Q[64] = {
		16, 11, 10, 16, 24, 40, 51, 61,
		12, 12, 14, 19, 26, 58, 60, 55,
		14, 13, 16, 24, 40, 57, 69, 56,
		14, 17, 22, 29, 51, 87, 80, 62,
		18, 22, 37, 56, 68, 109, 103, 77,
		24, 35, 55, 64, 81, 104, 113, 92,
		49, 64, 78, 87, 103, 121, 120, 101,
		72, 92, 95, 98, 112, 100, 103, 99
	};

	if (SDL_Init(SDL_INIT_VIDEO) < 0) {
		fprintf(stderr, "ERREUR - SDL_Init\n>>> %s\n", SDL_GetError());
		exit(EXIT_FAILURE);
	}

	atexit(SDL_Quit);

	if (!(img = IMG_Load("camol.jpg"))) {
		printf("Unable to open jpg file.\n");
		exit(EXIT_FAILURE);
	}

	printf("IMG FORMAT : %dx%d\n", img->w, img->h);

	window = SDL_SetVideoMode(img->w, img->h, 32, SDL_HWSURFACE | SDL_DOUBLEBUF);

	if (window == NULL) {
		fprintf(stderr, "ERREUR - impossible de passer en : %dx%dx%d\n>>> %s\n", 640, 480, 32, SDL_GetError());
		exit(EXIT_FAILURE);
	}

	window->format->BytesPerPixel = 4;
	SDL_WM_SetCaption("JPEG", NULL);

	while (!quit)
	{
		while (SDL_PollEvent(&e))
		{
			switch (e.type)
			{
			case SDL_QUIT:
				SDL_FreeSurface(window);
				SDL_FreeSurface(img);
				quit = 1;
				break;
			case SDL_KEYDOWN:
				if (e.key.keysym.sym == SDLK_ESCAPE)
					quit = 1;
				if (e.key.keysym.sym == SDLK_SPACE)
					setMatrix(window, img);
				if (e.key.keysym.sym == SDLK_p)
					pixelize(window, 5);
				if (e.key.keysym.sym == SDLK_s)
					smooth(window, 1);
				if (e.key.keysym.sym == SDLK_h)
					homogene(window, 0.1);
				if (e.key.keysym.sym == SDLK_n)
					negative(window);
				if (e.key.keysym.sym == SDLK_b)
					blackWhite(window, 0);
				if (e.key.keysym.sym == SDLK_f)
					fourierComp(window, 100, 8);
				if (e.key.keysym.sym == SDLK_c)
					chrominance(window, NULL, 0);
				if (e.key.keysym.sym == SDLK_j)
					JPEGComp(window, Q, 16, 4, 0.01, 1);
				if (e.key.keysym.sym == SDLK_r)
					JPEGDecomp(window);
				break;
			}
		}

	}

	return EXIT_SUCCESS;
}