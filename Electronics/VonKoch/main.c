#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "utils_SDL.h"
#include "graph.h"
#include "elec.h"

int Ttmp = 0;

void drawAll(screen s, list* L, point mPos1, point mPos2, int isMousePressed, int area_selection, elec* selection, point* pole_selection, icon* I, type typeMode) {
	SDL_Surface* w = s.w;

	SDL_FillRect(w, &(w->clip_rect), DDGREY);
	drawGrid(s);

	// Dessin des éléments
	elec* e;
	list* tmpL = L;
	while (tmpL) {
		e = tmpL->e;
		draw(s, *e->p1, *e->p2, e->t, LGREY, e->selected, 0);
		tmpL = tmpL->next;
	}

	// Zone de sélection.
	if (area_selection) {
		drawSelectedArea(w, mPos1, mPos2);
		selectZone(s, L, mPos1, mPos2);
	}

	point simuP1 = getSimuPoint(s, mPos1, 1);
	point simuP2 = getSimuPoint(s, mPos2, 0);
	// Déplacement des élémments selectionnés.
	if (selection && !(mPos1.x == mPos2.x && mPos1.y == mPos2.y)) {
		point v = sum(*selection->p2, *selection->p1, -1); // On garde en mémoire la "forme" de l'elec sélectionné.
		point p_ = sum(simuP2, setPoint((int)(v.x / 2), (int)(v.y / 2)), -1); // On lui fait suivre la souris.
		selection->p1->x = p_.x;
		selection->p1->y = p_.y;
		selection->p2->x = p_.x + v.x;
		selection->p2->y = p_.y + v.y;
	}

	// Détection si on survole un pôle
	point* poleTmp = NULL;
	if (pole_selection && !(mPos1.x == mPos2.x && mPos1.y == mPos2.y)) {
		drawBox(s, getScreenPoint(s, *pole_selection), CYAN);
		poleTmp = pole_selection;
		// TODO : parcours chaine utile ?
		while (poleTmp) { // Dans un sens de la chaîne...
			poleTmp->x = simuP2.x;
			poleTmp->y = simuP2.y;
			poleTmp = poleTmp->pnext_Connect;
		}
		poleTmp = pole_selection->pprec_Connect;
		while (poleTmp) { // ...Puis dans l'autre.
			poleTmp->x = simuP2.x;
			poleTmp->y = simuP2.y;
			poleTmp = poleTmp->pprec_Connect;
		}
	}
	// Si pas de pôle sélectionné, dessiner la boîte du pôle survolé en blanc.
	else if ((poleTmp = selectPole(L, mPos2, s))) drawBox(s, getScreenPoint(s, *poleTmp), WHITE);

	// Dessin de l'élément en cours de création
	if (isMousePressed && !selection) draw(s, simuP1, simuP2, typeMode, WHITE, 1, 0);

	drawCurrent(s, L, (int)(clock() / 100));
	if (Ttmp != (int)(clock() / 1000)) {
		Ttmp = (int)(clock() / 1000);
		printf("%ds\n", Ttmp);
	}


	drawGUI(s, I);

	SDL_Flip(w);

}

int main(int argc, char **argv)
{
	SDL_Surface * window;
	int quit = 0, pause = 0;
	SDL_Event e;
	int mPos1x = 0, mPos1y = 0, mPos2x = 0, mPos2y = 0, mPosx = 0, mPosy = 0;
	list* L = NULL;
	SDLKey k;
	point p, mPos1 = setPoint(mPos1x, mPos1y), mPos2;
	float zoomRatio;

	int isMousePressed = 0, shiftMode = 0, area_selection = 0;
	elec* selection = NULL;
	point* pole_selection = NULL;
	type typeMode = WIRE, tmpType = -1;

	if (SDL_Init(SDL_INIT_VIDEO) < 0) {
		fprintf(stderr, "ERREUR - SDL_Init\n>>> %s\n", SDL_GetError());
		exit(EXIT_FAILURE);
	}

	atexit(SDL_Quit);
	window = SDL_SetVideoMode(880, 620, 32, SDL_HWSURFACE | SDL_DOUBLEBUF);

	if (window == NULL) {
		fprintf(stderr, "ERREUR - impossible de passer en : %dx%dx%d\n>>> %s\n", 880, 620, 32, SDL_GetError());
		exit(EXIT_FAILURE);
	}

	screen s;
	s.zoom = 25;
	s.offsetx = 0;
	s.offsety = 0;
	s.w = window;

	icon* I = initIcons();

	SDL_WM_SetCaption("Elec", NULL);

	while (!quit)
	{
		while (SDL_PollEvent(&e))
		{
			switch (e.type)
			{
			case SDL_QUIT:
				quit = 1;
				break;
			case SDL_KEYDOWN:
				k = e.key.keysym.sym;
				if (k == SDLK_ESCAPE) quit = 1;
				// Suppression des éléments sélectionnés
				if (k == SDLK_DELETE) deleteSelected(&L);
				if (k == SDLK_LSHIFT || k == SDLK_RSHIFT) shiftMode = 1;
				// Navigation sur la map
				if (k == SDLK_UP) s.offsety -= 28;
				if (k == SDLK_DOWN) s.offsety += 28;
				if (k == SDLK_LEFT) s.offsetx -= 28;
				if (k == SDLK_RIGHT) s.offsetx += 28;
				// Zoom et Dézoom
				if (k == SDLK_w && s.zoom < 50) {
					s.zoom += 5;
					zoomRatio = (float)(s.zoom + 5) / s.zoom + 5;
					SDL_GetMouseState(&mPosx, &mPosy);
					p = getSimuPoint(s, setPoint(mPosx, mPosy), 0);
					s.offsetx += p.x * zoomRatio;
					s.offsety += p.y * zoomRatio;
				}
				if (k == SDLK_q && s.zoom > 5) {
					s.zoom -= 5;
					zoomRatio = (float)(s.zoom - 5) / s.zoom - 5;
					SDL_GetMouseState(&mPosx, &mPosy);
					p = getSimuPoint(s, setPoint(mPosx, mPosy), 0);
					s.offsetx += p.x * zoomRatio;
					s.offsety += p.y * zoomRatio;
				}
				break;
			case SDL_KEYUP:
				k = e.key.keysym.sym;
				if (k == SDLK_LSHIFT || k == SDLK_RSHIFT) shiftMode = 0;
				break;
			case SDL_MOUSEBUTTONDOWN:
				SDL_GetMouseState(&mPos1x, &mPos1y);
				if (mPos1x < 50 && (tmpType = selectType(I, setPoint(mPos1x, mPos1y))) != -1) {
					typeMode = tmpType;
					break;
				}

				mPos1x += s.offsetx;
				mPos1y += s.offsety;

				if (!shiftMode) deselectAll(L);
				mPos1 = setPoint(mPos1x, mPos1y);
				// Sélection d'un pôle
				if (shiftMode && (pole_selection = selectPole(L, mPos1, s))) {
					disconnect(pole_selection);
					break;
				}
				// Sélection d'un elec
				if (selection = select(L, getSimuPoint(s, mPos1, 1))) {
					disconnect(selection->p1);
					disconnect(selection->p2);
					break;
				}
				// Sélection de zone
				if (shiftMode) {
					area_selection = 1;
					break;
				}
				isMousePressed = 1;
				break;
			case SDL_MOUSEBUTTONUP:
				if (tmpType != -1) {
					tmpType = -1;
					break;
				}
				// Fin de sélection de zone
				if (area_selection) {
					area_selection = 0;
					break;
				}
				// Relâchement d'un pôle
				if (pole_selection) {
					tryConnect(L, pole_selection); // TODO suppr si p1 == p2
					pole_selection = NULL;
					break;
				}
				// Relâchement d'un elec
				if (selection) {
					tryConnect(L, selection->p1);
					tryConnect(L, selection->p2);
					selection = NULL;
					break;
				}
				// Ajout de l'elec dans la liste
				SDL_GetMouseState(&mPos2x, &mPos2y);
				addList(&L, createElec(L, getSimuPoint(s, mPos1, 1), getSimuPoint(s, setPoint(mPos2x, mPos2y), 0), typeMode));
				isMousePressed = 0;
				break;
			}
		}

		SDL_GetMouseState(&mPos2x, &mPos2y);
		if (!pause) drawAll(s, L, mPos1, setPoint(mPos2x, mPos2y), isMousePressed, area_selection, selection, pole_selection, I, typeMode);

	}
	freeList(L);
	freeIcons(I);

	return EXIT_SUCCESS;
}