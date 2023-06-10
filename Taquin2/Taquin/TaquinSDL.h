//////////////////////////////////////////////////////////////////////
//
//	JEU DE TAQUIN EN SDL
//
//	Utilisation de taquin.h/taquin.c pour augmenter les info et l'afficher en SDL
//	"Surcharge" des fonctions pour transposer le jeu en SDL
//  Ajout de la boucle de jeu graphique (gameLoop)
//
//////////////////////////////////////////////////////////////////////

#ifndef _TAQUIN_SDL_

#define _TAQUIN_SDL_

#include "taquin.h"
#include "SDL.h"

// Structure pour créer un taquin en SDL à partir d'une image de fond
typedef struct sTaquinSDL {
	Taquin taquin;					// taquin (cf. taquin.h)
	SDL_Surface * pWindow;			// fenêtre SDL qui contient le taquin
	SDL_Surface * pFond;			// image de fond du taquin
	int resX,resY;					// résolution d'une case du taquin
}TaquinSDL;


// Fonction permettant de créer le taquin en SDL à partir de sa taille et du fichier BMP à utiliser
// Cette fonction crée aussi la fenêtre SDL et la stocke dans la structure TaquinSDL
int createTaquinSDL(TaquinSDL * pTaquinSDL,int hauteur, int largeur, char * pathBMPfile);
// Fonction pour dessiner une case sur le fond
int displayCaseTaquin(TaquinSDL * pTaquinSDL,unsigned char caseTaquin, SDL_Rect * pDest, int x, int y, int refresh);
// fonction pour rendre le taquin dans son état actuel
int displayTaquinSDL(TaquinSDL * pTaquinSDL, int end);
// fonction permettant de faire le rendu SDL du taquin et de jouer (gestion des évènements à l'intérieur de la fonction)
int gameLoopSDL(int hauteur,int largeur, char * pathBMPfile, int minRandom, int maxRandom);
// fonction pour libérer le taquins et les surfaces SDL utilisées
int freeTaquinSDL(TaquinSDL * t);

int menu(TaquinSDL* t);


#endif