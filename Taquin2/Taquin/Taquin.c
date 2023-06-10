// Inclusion des bibliothèques utiles

#define _CRT_SECURE_NO_WARNINGS 1


#include "taquin.h"
#include "AStar.h"

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <conio.h>



// Fonction pour copier un plateau de taquin pSrc vers pDest
int copyTaquin(Taquin * pSrc, Taquin * pDest)
{
	for (int i = 0; i < pSrc->largeur; i++) {
		for (int j = 0; j < pSrc->hauteur; j++) {

			pDest->plateau[i][j] = pSrc->plateau[i][j];

		}
	}

	pDest->x = pSrc->x;
	pDest->y = pSrc->y;

	return 1;
}

// fonction qui renvoie 1 si les 2 taquins sont identiques
// 0 sinon
// -1 si il y a une erreur au passage des paramètres
int equalTaquin(Taquin * pTaquin1, Taquin * pTaquin2)
{
	if (!pTaquin1 || !pTaquin2) return -1;

	if (pTaquin1->hauteur != pTaquin2->hauteur || pTaquin1->largeur != pTaquin2->largeur || pTaquin1->x != pTaquin2->x || pTaquin1->y != pTaquin2->y) return 0;

	for (int i = 0; i < pTaquin1->largeur; i++) {
		for (int j = 0; j < pTaquin2->hauteur; j++) {

			if (pTaquin1->plateau[i][j] != pTaquin2->plateau[i][j]) return 0;

		}
	}

	return 1;
}

// Fonction qui crée un plateau de taquin 
// ATTENTION le plateau doit être NULL avant l'appel à cette fonction 
// Si le plateau n'est pas NULL la fonction essayera de libérer la zone mémoire occupée par le plateau et cela peut donc aboutir à un crash si le plateau n'a pas été initialisé
int createTaquin(Taquin * pTaquin, unsigned char hauteur, unsigned char largeur)
{
	// Test pour vérifier que les données passées ne sont pas corrompues
	if(!pTaquin) return 0;
	
	// Tests pour vérifier les paramètres de taille de taquin
	if(hauteur<SIZE_MINI) hauteur = SIZE_MINI;
	if(hauteur>SIZE_MAXI) hauteur = SIZE_MAXI;
	if(largeur<SIZE_MINI) largeur = SIZE_MINI;
	if(largeur>SIZE_MAXI) largeur = SIZE_MAXI;
	
	{
		int i;

		// On vérifie que le plateau n'existe pas
		// S'il existe on libère la mémoire avant de recréer le plateau
		if(pTaquin->plateau)
		{
			// On libère le plateau ligne par ligne
			for(i=0; i < pTaquin->hauteur; i++) if(pTaquin->plateau[i]) free(pTaquin->plateau[i]);
			// On libère le tableau qui stockait les lignes
			free(pTaquin->plateau);
			pTaquin->plateau = NULL;
		}

		pTaquin->hauteur = hauteur;
		pTaquin->largeur = largeur;

		// on alloue la zone mémoire pour stocker les adresses des lignes du tableau
		pTaquin->plateau = (unsigned char**) malloc(sizeof(unsigned char*)*hauteur);
		
		// si on a pas réussi à allouer la zone mémoire on retourne 0
		if(!pTaquin->plateau) return 0;

		for(i=0; i < hauteur; i++) pTaquin->plateau[i] = NULL;

		for(i=0; i < hauteur; i++)
		{
			// On alloue la zone mémoire pour contenir la ligne i
			pTaquin->plateau[i] = (unsigned char*) malloc(sizeof(unsigned char)*largeur);
			// S'il y a eu un souci à l'allocation on libère tout ce qui a déjà été alloué et on retourne 0
			if(!pTaquin->plateau[i])
			{
				freeTaquin(pTaquin);
				return 0;
			}
		}
		// On initialise le taquin
		if(!initTaquin(pTaquin)) return 0;
	}

	return 1;
}


// Fonction qui initialise le taquin
int initTaquin(Taquin * pTaquin)
{
	// TODO: initTaquin
	int cst = 0;
	for (int i = 0; i < pTaquin->largeur; i++) {
		for (int j = 0; j < pTaquin->hauteur; j++) {

			pTaquin->plateau[i][j] = cst;
			cst++;
		}
	}
	pTaquin->x = 0;
	pTaquin->y = 0;

	return 1;
}

// Fonction qui mélange le taquin en effectuant entre minRandom et maxRandom coups aléatoires
int mixTaquin(Taquin* pTaquin, int minRandom, int maxRandom)
{
	// TODO: mixTaquin

	srand(time(NULL));

	int ran = minRandom + rand() % (maxRandom - minRandom);

	int d = 0;

	for (int i = 0; i < ran; i++) {

		d = 0;

		//displayTaquin(pTaquin, 0);

		while (d == 0 || (pTaquin->y == pTaquin->largeur - 1 && d == 1) || (pTaquin->y == 0 && d == 2) || (pTaquin->x == pTaquin->hauteur - 1 && d == 3) || (pTaquin->x == 0 && d == 4)) {
			d = rand() % 5;
		}

		//printf("mixtaquin : i = %d, d = %d, h = %d\n", i, d, h(pTaquin));


		switch (d)
		{
		case 1:
			moveTaquin(pTaquin, GAUCHE);
			break;
		case 2:
			moveTaquin(pTaquin, DROITE);
			break;
		case 3:
			moveTaquin(pTaquin, HAUT);
			break;
		case 4:
			moveTaquin(pTaquin, BAS);
			break;
		}

	}

	return 1;
}

// Fonction qui permet de bouger une pièce du taquin (en bougeant la case vide)
int moveTaquin(Taquin* pTaquin, deplacement d)
{
	// TODO: moveTaquin
	if (!pTaquin) return 0;
	if (d == AUCUN || (pTaquin->y == pTaquin->largeur - 1 && d == 1) || (pTaquin->y == 0 && d == 2) || (pTaquin->x == pTaquin->hauteur - 1 && d == 3) || (pTaquin->x == 0 && d == 4))return 1;

	int d_c = 0;
	int d_r = 0;

	if (d == GAUCHE)d_c = -1;
	if (d == DROITE)d_c = 1;
	if (d == BAS)d_r = 1;
	if (d == HAUT)d_r = -1;

	pTaquin->plateau[pTaquin->x][pTaquin->y] = pTaquin->plateau[pTaquin->x - d_r][pTaquin->y - d_c];
	pTaquin->plateau[pTaquin->x - d_r][pTaquin->y - d_c] = 0;

	pTaquin->x -= d_r;
	pTaquin->y -= d_c;

	return 1;
}

// Fonction qui renvoie le déplacement à effectuer pour annuler le déplacement donné en paramètre
deplacement cancelMove(deplacement d)
{
	// TODO: cancelMove


	if (d == GAUCHE) d = DROITE;
	if (d == DROITE) d = GAUCHE;
	if (d == BAS)    d = HAUT;
	if (d == HAUT)   d = BAS;



	return d;
}

// Fonction qui renvoie 1 si le taquin es résolu, 0 sinon
int endTaquin(Taquin* pTaquin)
{
	// TODO: endTaquin
	int cst = 0;
	for (int i = 0; i < pTaquin->largeur; i++) {
		for (int j = 0; j < pTaquin->hauteur; j++) {
			if (pTaquin->plateau[i][j] != cst)return 0;
			//if (i != pTaquin->largeur - 1 && j != pTaquin->hauteur - 1 && pTaquin->plateau[i][j] != j + i * pTaquin->largeur) return 1;
			cst++;
		}
	}

	return 1;
}

// fonction d'affichage du taquin
int displayTaquin(Taquin * pTaquin, int offset)
{

	// TODO: displayTaquin
	for (int i = 0; i < pTaquin->largeur; i++) {
		for (int j = 0; j < pTaquin->hauteur; j++) {

			printf("[%d]  ", pTaquin->plateau[i][j]);

		}
		printf("\n");
	}

	return 1;
}

// Fonction pour libérer les zones mémoires occupées par un taquin
int freeTaquin(Taquin * pTaquin)
{
	// TODO: freeTaquin

	for (int i = 0; i < pTaquin->hauteur; i++) {
		free(pTaquin->plateau[i]);
	}
	free(pTaquin->plateau);
	free(pTaquin);

	return 1;
}

// Boucle de jeu 
int gameLoop(int hauteur,int largeur,int minRandom, int maxRandom)
{
	

	// BOUCLE DE JEU ! A DEVELOPPER
	Taquin* t = (Taquin*)calloc(1, sizeof(Taquin));

	createTaquin(t, hauteur, largeur);
	initTaquin(t);
	
	int touch = 0;
	deplacement d = AUCUN;

	// On mélange le taquin
	mixTaquin(t, minRandom, maxRandom);

	while(!_kbhit())
	{
		

		// On affiche le taquin mélangé
		displayTaquin(t, 0);
		printf("\n");
		printf("\n");
		printf("Rentrez un numeros entre 1 et 4 pour deplacer\n");
		printf("la case 0");
		printf("\n");
		printf("GAUCHE = 1\n");
		printf("DROITE = 2\n");
		printf("HAUT = 3\n");
		printf("BAS = 4\n");
		scanf("%d", &touch);
		printf("\n");
		printf("\n");

		if (touch == 0)d = AUCUN;
		if (touch == 1)d = DROITE;
		if (touch == 2)d = GAUCHE;
		if (touch == 3)d = BAS;
		if (touch == 4)d = HAUT;
		if (touch == 5)initTaquin(t);
		moveTaquin(t, d);

		
		if (endTaquin(t) == 1)break;
		//printf(".");
	}
	freeTaquin(t);
	return 0;
}

