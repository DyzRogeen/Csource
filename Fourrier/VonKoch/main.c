#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include "utils_SDL.h"

#define PI	3.14159265359

typedef struct sPoint {
	int x;
	int y;
}point;

point setPoint(float x, float y) {
	point p;
	p.x = x;
	p.y = y;
	return p;
}

float norm(point p) {
	return sqrtf(p.x * p.x + p.y * p.y);
}

float norm2(float x, float y) {
	return sqrtf(x * x + y * y);
}

void affFunc(SDL_Surface* window, int T, int height, float* f, Uint32 coul, float* t, float dt) {
	for (int x = 0; x < T - 1; x++) {
		_SDL_DrawLine(window, x, f[(x + (int)*t) % T] + height / 2, x + 1, f[(x + 1 + (int)*t) % T] + height / 2, coul);
	}
	if (dt == 0) return;
	*t += dt;
	if (*t > T) *t = 0;
	if (*t < 0) *t = T;

}

void affFig(SDL_Surface* window, int width, int height, int accuracy, float* Rs, Uint32 coul) {

	int o;
	for (o = 0; o < accuracy - 1; o++) {
		_SDL_DrawLine(window,
			Rs[o] * cos(o * 2 * PI / accuracy) + width / 2,
			Rs[o] * sin(o * 2 * PI / accuracy) + height / 2,
			Rs[o + 1] * cos((o + 1) * 2 * PI / accuracy) + width / 2,
			Rs[o + 1] * sin((o + 1) * 2 * PI / accuracy) + height / 2,
			coul);
	}
	_SDL_DrawLine(window,
		Rs[0] + width / 2,
		height / 2,
		Rs[o] * cos(o * 2 * PI / accuracy) + width / 2,
		Rs[o] * sin(o * 2 * PI / accuracy) + height / 2,
		coul);

}

void affGraph(SDL_Surface* window, point* g, int nbP, Uint32 coul) {

	if (!g) return;

	for (int i = 0; i < nbP - 1; i++) _SDL_DrawLine(window, g[i].x, g[i].y, g[i + 1].x, g[i + 1].y, coul);
	if (nbP > 1) _SDL_DrawLine(window, g[0].x, g[0].y, g[nbP - 1].x, g[nbP - 1].y, coul);

	SDL_UpdateRect(window, 0, 0, 0, 0);
	SDL_Flip(window);

}

void getCoefsf(float* f, float* a, float* b, int nbCoefs, int T) {

	float sumA, sumB;
	for (int n = 1; n < nbCoefs; n++) {

		sumA = 0;
		sumB = 0;
		for (int x = 0; x < T; x++) {
			sumA += f[x] * cos(2 * PI * x * n / T);
			sumB += f[x] * sin(2 * PI * x * n / T);
		}
		a[n] = sumA * 2 / T;
		b[n] = sumB * 2 / T;

	}

}

void getCoefsRs(float* Rs, float* a, float* b, int nbCoefs, int accuracy) {

	float sumA, sumB, ratio = 2 * PI / accuracy;
	for (int n = 1; n < nbCoefs; n++) {

		sumA = 0;
		sumB = 0;
		// (a + ib)(cos(wnt) - isin(wnt))
		for (int x = 0; x < accuracy; x++) {
			sumA += Rs[x] * (cos(x * ratio) * cos(x * (n - nbCoefs / 2) * ratio) + sin(x * ratio) * sin(x * (n - nbCoefs / 2) * ratio));
			sumB += Rs[x] * (sin(x * ratio) * cos(x * (n - nbCoefs / 2) * ratio) - cos(x * ratio) * sin(x * (n - nbCoefs / 2) * ratio));
		}
		a[n] = sumA / accuracy;
		b[n] = sumB / accuracy;
		if ((int)a[n] != 0 || (int)b[n] != 0) printf("%.1fcos(%dx) + %.1fisin(%dx)\n", a[n], n - nbCoefs / 2, b[n], n - nbCoefs / 2);

	}
	printf("\n");

}

void getCoefsGraph(point* g, int width, int height, float* a, float* b, int nbCoefs, int nbP) {

	float ratio = 2 * PI / nbP;
	float w, x, y;

	for (int n = 1; n < nbCoefs; n++) {

		for (int t = 0; t < nbP; t++) {
			x = g[t].x - width / 2;
			y = g[t].y - height / 2;
			w = t * ratio * (n - nbCoefs / 2);

			a[n] += x * cos(w) + y * sin(w);
			b[n] += y * cos(w) - x * sin(w);
		}
		a[n] /= nbP;
		b[n] /= nbP;
		if ((int)a[n] != 0 || (int)b[n] != 0) printf("%.1fcos(%dx) + %.1fisin(%dx)\n", a[n], n, b[n], n);
	}

}

void getFunc(float* f, float* a, float* b, int nbCoefs, int T) {

	for (int i = 0; i < T; i++) f[i] = 0;

	for (int n = 1; n < nbCoefs; n++) {
		for (int x = 0; x < T; x++) f[x] += a[n] * cos(2 * PI * x * n / T) + b[n] * sin(2 * PI * x * n / T);
	}
}

void getFig(float* Rs, float* a, float* b, int nbCoefs, int accuracy) {

	for (int i = 0; i < accuracy; i++) Rs[i] = 0;

	float ratio = 2 * PI / accuracy;
	for (int n = 1; n < nbCoefs; n++) {
		for (int x = 0; x < accuracy; x++) Rs[x] = norm2(
			Rs[x] * cos(x * ratio) + a[n] * cos(x * (n - nbCoefs / 2) * ratio) + b[n] * sin(x * (n - nbCoefs / 2) * ratio),
			Rs[x] * sin(x * ratio) - a[n] * sin(x * (n - nbCoefs / 2) * ratio) + b[n] * cos(x * (n - nbCoefs / 2) * ratio)
		);
	}
}

void getGraph(int width, int height, point* g, float* a, float* b, int nbCoefs, int nbP) {

	for (int i = 0; i < nbP; i++) g[i] = setPoint(width, height);

	float ratio = 2 * PI / nbP;
	float w;

	for (int n = 1; n < nbCoefs; n++) {

		for (int t = 0; t < nbP; t++) {
			w = t * ratio * (n - nbCoefs / 2);

			g[t].x += a[n] * cos(w) + b[n] * sin(w);
			g[t].y += b[n] * cos(w) - a[n] * sin(w);
		}
	}

}

void DrawCircle(SDL_Surface* window, float cn, int Posx, int Posy, Uint32 coul) {
	float ratio = 2 * PI / 100;
	for (int o = 0; o < 100; o++) {

		_SDL_DrawLine(window,
			Posx + cn * cos(o * ratio),
			Posy + cn * sin(o * ratio),
			Posx + cn * cos((o + 1) * ratio),
			Posy + cn * sin((o + 1) * ratio),
			coul);
	}
}

void figAnimation(SDL_Surface* window, short int* fig, int width, float* a, float* b, int n, int nbCoefs, int Posx, int Posy, Uint32 coul, float t) {

	//for (int i = 0; i < nbCoefs - n; i++) printf("a = %.1f b = %.1f n = %d (%d) and %d\n", a[i], b[i], n + i - nbCoefs / 2, n, (n >= nbCoefs || !a || !b));
	if (n >= nbCoefs || !a || !b) {
		if (Posx >= 0 && Posx < width) fig[Posx + Posy * width] = 1;
		return;
	}

	int Posx2 = Posx;
	int Posy2 = Posy;

	if ((int)*a != 0) {
		DrawCircle(window, *a, Posx, Posy, coul);
		Posx2 += (int)*a * cos(t * (n - nbCoefs / 2));
		Posy2 += (int)*a * sin(t * (n - nbCoefs / 2));
		_SDL_DrawLine(window, Posx, Posy, Posx2, Posy2, coul);
	}
	int Posx3 = Posx2;
	int Posy3 = Posy2;

	if ((int)*b != 0) {
		DrawCircle(window, *b, Posx2, Posy2, coul);
		Posx3 += (int)*b * cos(t * (n - nbCoefs / 2));
		Posy3 += (int)*b * sin(t * (n - nbCoefs / 2));
		_SDL_DrawLine(window, Posx2, Posy2, Posx3, Posy3, coul);
	}
	figAnimation(window, fig, width, a + 1, b + 1, n + 1, nbCoefs, Posx3, Posy3, coul, t);

}

void graphAnimation(SDL_Surface* window, short int* fig, int width, float* a, float* b, int n, int nbCoefs, int Posx, int Posy, Uint32 coul, float t) {

	//for (int i = 0; i < nbCoefs - n; i++) printf("a = %.1f b = %.1f n = %d (%d) and %d\n", a[i], b[i], n + i - nbCoefs / 2, n, (n >= nbCoefs || !a || !b));
	if (n >= nbCoefs || !a || !b) {
		if (Posx >= 0 && Posx < width) fig[Posx + Posy * width] = 1;
		return;
	}

	float c = norm2(*a, *b);

	float w;

	int Posx2 = Posx;
	int Posy2 = Posy;

	if ((int)c != 0) {
		//printf("azertyyyyyy a = %.1f et b = %.1f donc c = %.1f avec %.1f et %.1f\n",*a, *b, c, acos(*a / c) * (*b < 0 ? -1 : 1), (asin(*b / c) - PI * (*a < 0)));
		w = acos(*a / c) * (*b < 0 ? -1 : 1);
		DrawCircle(window, c, Posx, Posy, coul);
		Posx2 += (int)c * cos(t * (n - nbCoefs / 2) + w);
		Posy2 += (int)c * sin(t * (n - nbCoefs / 2) + w);
		_SDL_DrawLine(window, Posx, Posy, Posx2, Posy2, coul);
	}

	graphAnimation(window, fig, width, a + 1, b + 1, n + 1, nbCoefs, Posx2, Posy2, coul, t);

}

void trace(SDL_Surface* window, short int* fig, int width, int height, Uint32 coul) {

	for (int i = 0; i < width * height; i++) if (fig[i] == 1) _SDL_SetPixel(window, i % width, (int)(i / width), coul);

}

void updateScreenF(SDL_Surface* window, int width, int height, float* f, Uint32 coul, float* t, float dt) {

	SDL_FillRect(window, &(window->clip_rect), SDL_MapRGB(window->format, 0, 0, 0));
	affFunc(window, width, height, f, coul, t, dt);
	SDL_UpdateRect(window, 0, 0, 0, 0);
	SDL_Flip(window);

}

void updateScreenRs(SDL_Surface* window, int width, int height, int accuracy, float* Rs, Uint32 coul) {
	SDL_FillRect(window, &(window->clip_rect), SDL_MapRGB(window->format, 0, 0, 0));
	affFig(window, width, height, accuracy, Rs, coul);
	SDL_UpdateRect(window, 0, 0, 0, 0);
	SDL_Flip(window);
}

int findAngle(point mPos, int accuracy, int width) {
	int o = (int)(acos(mPos.x / norm(mPos)) * accuracy / (2 * PI));
	return (mPos.y > 0 ? o : (mPos.y == 0 && mPos.x < width / 2 ? accuracy / 2 : accuracy - o));
}

int main(int argc, char** argv)
{
	SDL_Surface* window;
	int quit = 0;
	int mode = 1;
	int PlayAnim = 0;
	int mDown = 0;
	int n = 0;
	int pCnt = 0;
	int nbP = 0;
	int nbCoefs = 50;
	SDL_Event e;
	Uint8 mBtn;
	point mPos;

	int w = 640, h = 480;
	int accuracy = 100;
	float dt = 0;
	float t = 0;
	int reset = 0;

	float* f = (float*)calloc(w, sizeof(float));
	float* f2 = (float*)calloc(w, sizeof(float));
	float* a = (float*)calloc(nbCoefs, sizeof(float));
	float* b = (float*)calloc(nbCoefs, sizeof(float));

	float* Rs = (float*)calloc(accuracy, sizeof(float));
	float* Rs2 = (float*)calloc(accuracy, sizeof(float));

	point* g = (point*)calloc(1, sizeof(point));
	point* g2 = (point*)calloc(1, sizeof(point));
	short int* fig = (short int*)calloc(w * h, sizeof(short int));
	for (int i = 0; i < w * h; i++) fig[i] = 0;

	for (int i = 1; i <= w; i++) f[i - 1] = h / 2 - i * h / w;
	// f[i - 1] = (i < w / 2 ? -h / 2 : h / 2);
	// f[i - 1] = h/2 - i*h/w;
	for (int i = 0; i < accuracy; i++) Rs[i] = 30;

	// Initialisation
	if (SDL_Init(SDL_INIT_VIDEO) < 0) {
		fprintf(stderr, "ERREUR - SDL_Init\n>>> %s\n", SDL_GetError());
		exit(EXIT_FAILURE);
	}

	// Permet de quitter proprement par la suite (revenir à la résolution initiale de l'écran avec le bon format vidéo)
	atexit(SDL_Quit);
	// Initialise le mode vidéo de la fenêtre
	window = SDL_SetVideoMode(640, 480, 32, SDL_HWSURFACE | SDL_DOUBLEBUF);

	// Si on n'a pas réussi à créer la fenêtre on retourne une erreur
	if (window == NULL) {
		fprintf(stderr, "ERREUR - impossible de passer en : %dx%dx%d\n>>> %s\n", w, h, 32, SDL_GetError());
		exit(EXIT_FAILURE);
	}

	Uint32 coul = SDL_MapRGB(window->format, 255, 255, 255);
	Uint32 coulR = SDL_MapRGB(window->format, 255, 0, 0);

	affFunc(window, w, h, f, coul, &t, 0);
	SDL_UpdateRect(window, 0, 0, 0, 0);
	SDL_Flip(window);

	// On met le titre sur la fenêtre
	SDL_WM_SetCaption("Fourrier", NULL);

	while (!quit)
	{
		while (SDL_PollEvent(&e))
		{
			switch (e.type)
			{
			case SDL_QUIT:
				quit = 1;
				free(f);
				free(f2);
				free(a);
				free(b);
				free(Rs);
				free(Rs2);
				free(g);
				free(g2);
				free(fig);
				break;
			case SDL_KEYDOWN:
				if (e.key.keysym.sym == SDLK_ESCAPE)
					quit = 1;
				if (e.key.keysym.sym == SDLK_SPACE) {
					if (mode == 1) {
						getCoefsf(f, a, b, nbCoefs, w);
						getFunc(f, a, b, nbCoefs, w);
						updateScreenF(window, w, h, f, coul, &t, 0);
					}
					else if (mode == 2) {
						getCoefsRs(Rs, a, b, nbCoefs, accuracy);
						getFig(Rs, a, b, nbCoefs, accuracy);
						updateScreenRs(window, w, h, accuracy, Rs, coul);
					}
					else {
						for (int i = 0; i < w * h; i++) fig[i] = 0;

						getCoefsGraph(g, w, h, a, b, nbCoefs, nbP);
 						g2 = (point*)realloc(g2, sizeof(g));

						getGraph(w / 2, h / 2, g2, a, b, nbCoefs, nbP);
						SDL_FillRect(window, &(window->clip_rect), SDL_MapRGB(window->format, 0, 0, 0));
						affGraph(window, g2, nbP, coul);
					}

				}
				if (e.key.keysym.sym == SDLK_RIGHT) {
					n++;
					for (int x = 0; x < w; x++) f2[x] = a[n] * cos(2 * PI * x * n / w) + b[n] * sin(2 * PI * x * n / w);

					updateScreenF(window, w, h, f2, coul, &t, 0);
					printf("Visualizing :\nY = %.2f * cos(2PI * %d) + %.2f * cos(2PI * %d)\n\n", a[n], n, b[n], n);
				}
				if (e.key.keysym.sym == SDLK_LEFT) {

					if (n <= 1) {
						n = 0;
						updateScreenF(window, w, h, f, coul, &t, 0);
						printf("Visualizing :\nOriginal signal\n\n");
						break;
					}
					n--;
					for (int x = 0; x < w; x++) f2[x] = a[n] * cos(2 * PI * x * n / w) + b[n] * sin(2 * PI * x * n / w);

					updateScreenF(window, w, h, f2, coul, &t, 0);
					printf("Visualizing :\nY = %.2f * cos(2PI * %d) + %.2f * cos(2PI * %d)\n\n", a[n], n, b[n], n);
				}
				if (e.key.keysym.sym == SDLK_DOWN) {
					if (n == nbCoefs) break;
					n++;
					if (mode == 1) {
						getFunc(f2, a, b, n, w);

						updateScreenF(window, w, h, f2, coul, &t, 0);
						printf("Visualizing :\nSum of the %d first(s) signal(s)\n\n", n);
					}
					else if (mode == 2) {
						getFig(Rs2, a, b, n, accuracy);

						updateScreenRs(window, w, h, accuracy, Rs2, coul);
						printf("Visualizing :\nSum of the %d first(s) figure(s)\n\n", n);
					}
					else {
						getGraph(w / 2, h / 2, g2, a, b, n, nbP);
						SDL_FillRect(window, &(window->clip_rect), SDL_MapRGB(window->format, 0, 0, 0));
						affGraph(window, g2, nbP, coul);
					}

				}
				if (e.key.keysym.sym == SDLK_UP) {

					if (n <= 1) {
						n = 0;
						if (mode == 1) {
							updateScreenF(window, w, h, f, coul, &t, 0);
							printf("Visualizing :\nOriginal signal\n\n");
						}
						else if (mode == 2) {
							updateScreenRs(window, w, h, accuracy, Rs, coul);
							printf("Visualizing :\nOriginal figure\n\n");
						}
						else {
							getGraph(w / 2, h / 2, g2, a, b, nbCoefs, nbP);
							SDL_FillRect(window, &(window->clip_rect), SDL_MapRGB(window->format, 0, 0, 0));
							affGraph(window, g2, nbP, coul);
						}

						break;
					}
					n--;
					if (mode == 1) {
						getFunc(f2, a, b, n, w);
						updateScreenF(window, w, h, f2, coul, &t, 0);
						printf("Visualizing :\nSum of the %d first(s) signal(s)\n\n", n);
					}
					else if (mode == 2) {
						getFig(Rs2, a, b, n, accuracy);
						updateScreenRs(window, w, h, accuracy, Rs2, coul);
						printf("Visualizing :\nnSum of the %d first(s) figure(s)\n\n");
					}
					else {
						getGraph(w / 2, h / 2, g2, a, b, n, nbP);
						SDL_FillRect(window, &(window->clip_rect), SDL_MapRGB(window->format, 0, 0, 0));
						affGraph(window, g2, nbP, coul);
					}
				}
				if (e.key.keysym.sym == SDLK_p) dt += 0.2;
				if (e.key.keysym.sym == SDLK_o) dt -= 0.2;
				if (e.key.keysym.sym == SDLK_r) reset = 1;
				if (e.key.keysym.sym == SDLK_q && mode == 3) PlayAnim = !PlayAnim;

				if (e.key.keysym.sym == SDLK_2) { updateScreenF(window, w, h, f, coul, &t, 0); mode = 1; }
				if (e.key.keysym.sym == SDLK_3) { updateScreenRs(window, w, h, accuracy, Rs, coul); mode = 2; }
				if (e.key.keysym.sym == SDLK_4) { mode = 3; }

				break;
			case SDL_MOUSEBUTTONDOWN:
				mDown = 1;
				break;
			case SDL_MOUSEBUTTONUP:
				mDown = 0;
				break;
			}
		}

		if (PlayAnim) {
			t += 0.001;
			if (t > 2 * PI) t = 0;

			SDL_FillRect(window, &(window->clip_rect), SDL_MapRGB(window->format, 0, 0, 0));
			if (mode == 2) figAnimation(window, fig, w, a, b, 0, (n < 2 ? nbCoefs : n), w / 2, h / 2, coul, t);
			else graphAnimation(window, fig, w, a, b, 0, (n < 2 ? nbCoefs : n), w / 2, h / 2, coul, t);
			trace(window, fig, w, h, coulR);
			affGraph(window, g2, nbP, coul);
		}
		if (dt != 0) {
			if (reset) reset = t = dt = 0;

			if (n == 0) updateScreenF(window, w, h, f, coul, &t, dt);
			else updateScreenF(window, w, h, f2, coul, &t, dt);
		}
		if (mDown)
		{
			// On vide la fenêtre (on la remplit en noir)
			SDL_FillRect(window, &(window->clip_rect), SDL_MapRGB(window->format, 0, 0, 0));

			mBtn = SDL_GetMouseState(&mPos.x, &mPos.y);
			if (mode == 1) {
				f[mPos.x] = mPos.y - h / 2;
				affFunc(window, w, h, f, coul, &t, 0);
			}
			else if (mode == 2) {
				mPos.x -= w / 2;
				mPos.y -= h / 2;
				Rs[findAngle(mPos, accuracy, w)] = norm(mPos);
				affFig(window, w, h, accuracy, Rs, coul);
			}
			else if (!fig[mPos.x + mPos.y * w]) {
				fig[mPos.x + mPos.y * w] = 1;
				if (pCnt == 0) {
					g = (point*)realloc(g, (nbP + 1) * sizeof(point));
					g[nbP] = setPoint(mPos.x, mPos.y);
					affGraph(window, g, nbP, coul);
					nbP++;
				}
				pCnt = (pCnt + 1) % 5;
			}

			if (mode != 3) {
				SDL_UpdateRect(window, 0, 0, 0, 0);
				SDL_Flip(window);
			}
		}

	}

	return EXIT_SUCCESS;
}