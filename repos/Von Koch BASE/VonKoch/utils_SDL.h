#include "../SDL-1.2.15/include/SDL.h"

/**	
 *	@brief	Fonction permettant de dessiner un pixel dans une fenêtre SDL.
 *	
 *	@param	p_affichage		Fenêtre sur laquelle le pixel doit être affiché.
 *	@param	p_x				Abscisse du pixel à afficher.
 *	@param	p_y				Ordonnée du pixel à afficher.
 *	@param	p_coul			Couleur du pixel à afficher.
 */
void _SDL_SetPixel(SDL_Surface* p_affichage, int p_x, int p_y, Uint32 p_coul);

/**	
 *	@brief	Fonction permettant de dessiner un segment dans une fenetre SDL.
 *			Cette fonction utilise la fonction _SDL_SetPixel.
 *	
 *	@param	p_affichage		Fenêtre sur laquelle on souhaite afficher le segment.
 *	@param	p_x1			Abscisse de départ du segment à afficher.
 *	@param	p_y1			Ordonnée de départ du segment à afficher.
 *	@param	p_x2			Abscisse d'arrivée du segment à afficher.
 *	@param	p_y2			Ordonnée d'arrivée du segment à afficher.
 *	@param	p_red			Composante rouge de la couleur du segment à afficher.
 *	@param	p_green			Composante verte de la couleur du segment à afficher.
 *	@param	p_blue			Composante bleue de la couleur du segment à afficher.
 *	
 *	@see	_SDL_SetPixel
 */
void _SDL_DrawLine(SDL_Surface * p_affichage, int p_x1, int p_y1, int p_x2, int p_y2, const Uint8 p_red, const Uint8 p_green, const Uint8 p_blue);
