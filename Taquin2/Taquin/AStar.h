//////////////////////////////////////////////////////////////////////
//
//	AStar / A*
//
//	Structures de données utiles à la représentation de liste pour la résolution AStar du taquin
//	Primitives pour insérer un élément dans une liste triée et pour prélever en tête de liste
//
//////////////////////////////////////////////////////////////////////

#ifndef _ASTAR_

#define _ASTAR_


// On a besoin des structures et des fonctions pour gérer le taquin, on inclut donc la biblilothèque
#include "Taquin.h"
#include "SDL.h"

// Structure de donnée pour réprésenter un noeud de liste en mémoire
typedef struct sListAStar {
	int g;						// Distance déjà parcourue entre l'état initial et l'état courant
	int h;						// Evaluation de la distance à parcourir entre l'état initial et l'état final
	Taquin* Taq;				// Taquin contenu dans le noeud courant
	deplacement d;				// dernier déplacement effectué pour arriver à ce taquin
	struct sListAStar* LastNode;// Pointeur vers le noeud correspondant à l'étape précédente dans la solution
	struct sListAStar* next;	// Pointeur vers le noeud suivant (liste simplement chaînée)
}ListAStar;

typedef ListAStar* ptrListAStar;		// Redéfinition pour faciliter l'écriture dans les en-têtes des fonctions

// fonction pour créer (allouer) un noeud de liste et l'initialiser avec le taquin passé en paramètre
ptrListAStar createNodeList(Taquin* pTaquin, int gValue, int fValue, deplacement d, ptrListAStar pPrevPlay);

// Insertion dans la liste de façon triée ou non
// si on passe le paramètre tri à 0, on insère en tête de liste
int insertList(ptrListAStar* ppHead, ptrListAStar pNode, int tri);

// Fonction pour prélever le noeud en tête de liste
// Retourne le noeud prélevé
ptrListAStar popList(ptrListAStar* ppHead);

// fonction qui retourne le noeud dans lequel on trouve le taquin passé en paramètre (pointeur sur le pointeur dans la liste)
ptrListAStar* isInList(ptrListAStar* ppHead, Taquin* pTaquin);

// fonction pour afficher une liste
// si on met displayFGH à 0 les valeur de F, G et H ne sont pas affichées
int displayList(ptrListAStar pHead, int displayFGH);

void freeList(ptrListAStar* list);

// Fonction pour résoudre le taquin en utilisant l'algorithme A*
// La fonction prend comme taquin initial le taquin spécifié par pTaquin
// elle remplit 
//   - pTabDeplacement (un tableau contenant les déplacements à effectuer pour résoudre le taquin)
//   - pNbDeplacements
//   - pNbTaquinsGeneres
//   - pTimeElapsed
// Si stepByStep est différent de 0 on affiche dans la console toutes les étapes de la résolution
// pWindow
int solveTaquin(Taquin* pTaquin, deplacement** pTabDeplacement, unsigned long* pNbDeplacements, unsigned long* pNbTaquinsGeneres, unsigned long* pTimeElapsed, int stepByStep, SDL_Surface* pWindow);

// fonction d'évaluation pour la résolution avec AStar
int h(Taquin* pTaquin);


#endif