#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include "utils_SDL.h"

#define PI	3.14159265359

typedef struct sPoint{
	int m_x;
	int m_y;
}point;

/**
 *	@brief	Fonction permettant de calculer la norme d'un vecteur.
 *
 *	@param	v		Vecteur dont on souhaite calculer la norme.
 *	@return			norme du vecteur passé en paramètre
 */
float norm(point v)
{
	return sqrtf(v.m_x * v.m_x + v.m_y * v.m_y);
}

/**
 *	@brief	Fonction permettant de dessiner une courbe de Koch.
 *	La courbe de Von Koch sera dessiné selon ce schéma :
 *
 *						  e
 *						 / \
 *					    /   \
 *				p_a----c     d----p_b
 *
 *	Cette fonction utilise la méthode de la rotation pour calculer les semgment CE et ED
 *
 *	@param	p_affichage		Fenêtre sur laquelle on souhaite dessiner.
 *	@param	p_iteration		Nombre d'itération que l'on souhaite appliquer à la courbe.
 *	@param	p_a				Point de départ du segment.
 *	@param	p_b				Point d'arrivée du segment.
 */
void vonKoch_rot(SDL_Surface* p_affichage, int p_iteration, point p_a, point p_b)
{
	point c, d, e, f;
	point AB, CE;

	if (!p_iteration)
	{
		_SDL_DrawLine(p_affichage, p_a.m_x, p_a.m_y, p_b.m_x, p_b.m_y, 255, 255, 255);
		return;
	}

	AB.m_x = p_b.m_x - p_a.m_x;
	AB.m_y = p_b.m_y - p_a.m_y;

	c.m_x = p_a.m_x + AB.m_x / 3;
	c.m_y = p_a.m_y + AB.m_y / 3;

	d.m_x = p_a.m_x + 2 * AB.m_x / 3;
	d.m_y = p_a.m_y + 2 * AB.m_y / 3;

	// CE est la rotation de AB de 60° divisée par 3
	CE.m_x = (AB.m_x * cos(-PI / 3.0f) - AB.m_y * sin(-PI / 3.0f)) / 3;
	CE.m_y = (AB.m_x * sin(-PI / 3.0f) + AB.m_y * cos(-PI / 3.0f)) / 3;

	e.m_x = c.m_x + CE.m_x;
	e.m_y = c.m_y + CE.m_y;

	vonKoch_rot(p_affichage, p_iteration - 1, p_a, c);
	vonKoch_rot(p_affichage, p_iteration - 1, c, e);
	vonKoch_rot(p_affichage, p_iteration - 1, e, d);
	vonKoch_rot(p_affichage, p_iteration - 1, d, p_b);
}

/**
 *	@brief	Fonction permettant de dessiner une courbe de Koch.
 *	La courbe de Von Koch sera dessiné selon ce schéma :
 *
 *						  e
 *						 /|\
 *					    / | \
 *				p_a----c  e' d----p_b
 *
 *	Cette fonction utilise E', le projeté de E sur AB pour calculer la position de E et donc les segments CE et ED
 *
 *	@param	p_affichage		Fenêtre sur laquelle on souhaite dessiner.
 *	@param	p_iteration		Nombre d'itération que l'on souhaite appliquer à la courbe.
 *	@param	p_a				Point de départ du segment.
 *	@param	p_b				Point d'arrivée du segment.
 */
void vonKoch_proj(SDL_Surface* p_affichage, int p_iteration, point p_a, point p_b)
{
	point c, d, e, f, e2;
	point AB, CE, n;

	if (!p_iteration)
	{
		_SDL_DrawLine(p_affichage, p_a.m_x, p_a.m_y, p_b.m_x, p_b.m_y, 255, 255, 255);
		return;
	}

	AB.m_x = p_b.m_x - p_a.m_x;
	AB.m_y = p_b.m_y - p_a.m_y;

	c.m_x = p_a.m_x + AB.m_x / 3;
	c.m_y = p_a.m_y + AB.m_y / 3;

	d.m_x = p_a.m_x + 2 * AB.m_x / 3;
	d.m_y = p_a.m_y + 2 * AB.m_y / 3;

	e2.m_x = p_a.m_x + AB.m_x / 2;
	e2.m_y = p_a.m_y + AB.m_y / 2;

	n.m_x = AB.m_y;
	n.m_y = -AB.m_x;

	float dist;
	dist = sqrt(pow(norm(AB) / 3, 2) - pow(norm(AB) / 6, 2));

	n.m_x *= dist / norm(AB);
	n.m_y *= dist / norm(AB);

	e.m_x = e2.m_x + n.m_x;
	e.m_y = e2.m_y + n.m_y;

	vonKoch_proj(p_affichage, p_iteration - 1, p_a, c);
	vonKoch_proj(p_affichage, p_iteration - 1, c, e);
	vonKoch_proj(p_affichage, p_iteration - 1, e, d);
	vonKoch_proj(p_affichage, p_iteration - 1, d, p_b);
}

int main(int argc, char **argv)
{
	point a, b, c;
	SDL_Surface * window;
	int iterations = 0;
	int quit = 0;
	enum { dessin1, dessin2, rendu } state = dessin1;
	SDL_Event e;
	Uint8 mouseButton;
	point mousePos;
	int flocon = 0;

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
		fprintf(stderr, "ERREUR - impossible de passer en : %dx%dx%d\n>>> %s\n", 640, 480, 32, SDL_GetError());
		exit(EXIT_FAILURE);
	}

	// On met le titre sur la fenêtre
	SDL_WM_SetCaption("VonKoch", NULL);

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
				if (e.key.keysym.sym == SDLK_ESCAPE)
					quit = 1;
				if (e.key.keysym.sym == SDLK_a)
					iterations++;
				if (e.key.keysym.sym == SDLK_q)
				{
					iterations--;
					iterations = iterations >= 0 ? iterations : 0;
				}
				if (e.key.keysym.sym == SDLK_f)															// L'appuie sur F active/désactive l'aspect "flocon"
					flocon = (flocon+1) % 2;
				break;
			case SDL_MOUSEBUTTONDOWN:
				mouseButton = SDL_GetMouseState(&mousePos.m_x, &mousePos.m_y);

				switch (state)
				{
				case dessin1:
					a.m_x = mousePos.m_x;
					a.m_y = mousePos.m_y;
					state = dessin2;
					break;
				case dessin2:
					b.m_x = mousePos.m_x;
					b.m_y = mousePos.m_y;
					c.m_x = a.m_x + ((b.m_x - a.m_x) * cos(PI / 3.0f) - (b.m_y - a.m_y) * sin(PI / 3.0f));
					c.m_y = a.m_y + ((b.m_x - a.m_x) * sin(PI / 3.0f) + (b.m_y - a.m_y) * cos(PI / 3.0f));
					state = rendu;
					break;
				case rendu:
				default:
					a.m_x = mousePos.m_x;
					a.m_y = mousePos.m_y;
					state = dessin2;
					iterations = 0;
					break;
				}

				break;
			}
		}

		if (state == rendu)
		{
			// On vide la fenêtre (on la remplit en noir)
			SDL_FillRect(window, &(window->clip_rect), SDL_MapRGB(window->format, 0, 0, 0));

//			vonKoch_rot(window, iterations, a, b);
			vonKoch_proj(window, iterations, a, b);
			if (flocon)
			{
				vonKoch_rot(window, iterations, b, c);
				vonKoch_rot(window, iterations, c, a);
			}

			// Met à jour le buffer de la fenêtre
			SDL_UpdateRect(window, 0, 0, 0, 0);
			// Flip le buffer pour l'envoyer vers l'écran (la fenêtre est configurée en double buffer)
			SDL_Flip(window);
		}

	}

	return EXIT_SUCCESS;
}