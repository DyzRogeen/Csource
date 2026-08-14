#include "utils_SDL.h"


void _SDL_SetPixel(SDL_Surface* p_affichage, int p_x, int p_y, Uint32 p_coul)
{
	// test si le pixel est dans la fenêtre avant de le dessiner si besoin
	if( (p_x>=0 && p_x<p_affichage->clip_rect.w) && (p_y>=0 && p_y<p_affichage->clip_rect.h))  *((Uint32*)(p_affichage->pixels) + p_x + p_y * p_affichage->w) = p_coul;
}

void _SDL_DrawLine(SDL_Surface * p_affichage, int p_x1, int p_y1, int p_x2, int p_y2, const Uint8 p_red, const Uint8 p_green, const Uint8 p_blue)
{
  int d, dx, dy, aincr, bincr, xincr, yincr, x, y;
  Uint32 coul = SDL_MapRGB(p_affichage->format, p_red, p_green, p_blue);
  // On regarde l'inclinaison de la ligne pour savoir si une abscisse a plusieurs ordonnées ou l'inverse

  // Angle > 45°
  if (abs(p_x2 - p_x1) < abs(p_y2 - p_y1)) {
		/* parcours par l'axe vertical */

		
		if (p_y1 > p_y2) {
			int temp;
			temp = p_x1;
			p_x1 = p_x2;
			p_x2 = temp;

			temp = p_y1;
			p_y1 = p_y2;
			p_y2 = temp;
		}

		xincr = p_x2 > p_x1 ? 1 : -1;
		dy = p_y2 - p_y1;
		dx = abs(p_x2 - p_x1);
		d = 2 * dx - dy;
		aincr = 2 * (dx - dy);
		bincr = 2 * dx;
		x = p_x1;
		y = p_y1;

		_SDL_SetPixel(p_affichage,x, y, coul);

		for (y = p_y1+1; y <= p_y2; ++y) {
		  if (d >= 0) {
			x += xincr;
			d += aincr;
		  } else
			d += bincr;

		  _SDL_SetPixel(p_affichage,x, y, coul);
		}

  // Angle < 45°
  } else {
    /* parcours par l'axe horizontal */
    
    if (p_x1 > p_x2) {
		int temp;
		temp = p_x1;
		p_x1 = p_x2;
		p_x2 = temp;

		temp = p_y1;
		p_y1 = p_y2;
		p_y2 = temp;
    }

    yincr = p_y2 > p_y1 ? 1 : -1;
    dx = p_x2 - p_x1;
    dy = abs(p_y2 - p_y1);
    d = 2 * dy - dx;
    aincr = 2 * (dy - dx);
    bincr = 2 * dy;
    x = p_x1;
    y = p_y1;

    _SDL_SetPixel(p_affichage,x, y, coul);

    for (x = p_x1+1; x <= p_x2; ++x) {
      if (d >= 0) {
			y += yincr;
			d += aincr;
      } else
			d += bincr;

      _SDL_SetPixel(p_affichage,x, y, coul);
    }
  }    
}


