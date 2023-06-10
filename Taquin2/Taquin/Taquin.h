//////////////////////////////////////////////////////////////////////
//
//	JEU DE TAQUIN
//
//	Structures de données utiles à la représentation du jeu
//	Fonctions utiles pour jouer au taquin en mode console
//
//////////////////////////////////////////////////////////////////////

#ifndef _TAQUIN_

#define _TAQUIN_


// Nombre de cases maximum en largeur ou hauteur
#define SIZE_MAXI	9
// Nombre de cases minimum en largeur ou hauteur
#define SIZE_MINI	2

// Case vide, on définit la case vide comme étant la valeur 0, elle doit être en haut à gauche du taquin quand il est terminé
#define VIDE 0


// Type redéfini pour sélectionner le déplacement à effectuer
typedef enum{AUCUN, GAUCHE, DROITE, HAUT, BAS} deplacement;

// Structure de données pour stocker le taquin et ses possibilités de jeu (position de la case vide)
typedef struct sTaquin {
	unsigned char ** plateau;			// Tableau contenant le taquin (
	unsigned char hauteur,largeur;		// Taille du plateau
	unsigned char x,y;					// Position de la case vide à déplacer
} Taquin;


// Fonction pour copier un plateau de taquin pSrc vers pDest
int copyTaquin(Taquin * pSrc, Taquin * pDest);

// fonction qui renvoie 1 si les 2 taquins sont identiques
// 0 sinon
// -1 si il y a une erreur au passage des paramètres
int equalTaquin(Taquin * pTaquin1, Taquin * pTaquin2);

// Fonction qui crée un plateau de taquin 
// ATTENTION le plateau doit être NULL avant l'appel à cette fonction 
// Si le plateau n'est pas NULL la fonction essayera de libérer la zone mémoire occupée par le plateau et cela peut donc aboutir à un crash si le plateau n'a pas été initialisé
int createTaquin(Taquin * pTaquin, unsigned char hauteur, unsigned char largeur);

// Fonction qui initialise le taquin
int initTaquin(Taquin * pTaquin);

// Fonction qui mélange le taquin en effectuant entre minRandom et maxRandom coups aléatoires
int mixTaquin(Taquin * pTaquin, int minRandom, int maxRandom);

// Fonction qui permet de bouger une pièce du taquin (en bougeant la case vide)
int moveTaquin(Taquin * pTaquin, deplacement d);

// Fonction qui renvoie le déplacement à effectuer pour annuler le déplacement donné en paramètre
deplacement cancelMove(deplacement d);

// Fonction qui renvoie 1 si le taquin est résolu, 0 sinon
int endTaquin(Taquin * pTaquin);

// fonction d'affichage du taquin
int displayTaquin(Taquin * pTaquin, int offset);

// Fonction pour libérer les zones mémoires occupées par un taquin
int freeTaquin(Taquin * pTaquin);

// Boucle de jeu 
int gameLoop(int hauteur,int largeur,int minRandom, int maxRandom);



#endif