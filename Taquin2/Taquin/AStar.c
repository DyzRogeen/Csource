#include "AStar.h"
#include <windows.h>
#include <stdlib.h>
#include <stdio.h>
#include <conio.h>
#include <math.h>
#include "SDL.h"
#include "SDL_image.h"



// fonction pour créer (allouer) un noeud de liste et l'initialiser avec le taquin passé en paramètre
ptrListAStar createNodeList(Taquin* pTaquin, int gValue, int fValue, deplacement d, ptrListAStar pPrevPlay)
{
	ListAStar* new = (ListAStar*)calloc(1, sizeof(ListAStar));

	new->Taq = (Taquin*)calloc(1, sizeof(Taquin));
	createTaquin(new->Taq, pTaquin->hauteur, pTaquin->largeur);
	copyTaquin(pTaquin, new->Taq);

	new->d = d;
	new->g = gValue;
	new->h = fValue - gValue;
	new->LastNode = pPrevPlay;
	new->next = NULL;
	return new;
}

// Insertion dans la liste de façon triée ou non
// si on passe le paramètre tri à 0, on insère en tête de liste
int insertList(ptrListAStar* ppHead, ptrListAStar pNode, int tri)
{
	if (!ppHead) return 0;

	if (!*ppHead) {
		*ppHead = pNode;
		return 0;
	}

	if (tri == 0) {
		pNode->next = (*ppHead);
		(*ppHead) = pNode;
		return 0;
	}

	ptrListAStar tmp = *ppHead;

	if (tmp && tmp->h > pNode->h) {
		pNode->next = *ppHead;
		*ppHead = pNode;
		return 0;
	}

	while (tmp->next && tmp->next->h < pNode->h) tmp = tmp->next;

	pNode->next = tmp->next;
	tmp->next = pNode;

	return 1;
}

// Fonction pour prélever le noeud en tête de liste
// Retourne le noeud prélevé
ptrListAStar popList(ptrListAStar* ppHead)
{
	ptrListAStar tmp = createNodeList((*ppHead)->Taq, (*ppHead)->g, (*ppHead)->g + (*ppHead)->h, (*ppHead)->d, (*ppHead)->LastNode);

	*ppHead = (*ppHead)->next;
	return tmp;

}

// fonction qui retourne le noeud dans lequel on trouve le taquin passé en paramètre (pointeur sur le pointeur dans la liste)
ptrListAStar* isInList(ptrListAStar* ppHead, Taquin* pTaquin)
{

	ptrListAStar tmp = *ppHead;

	while (tmp) {

		if (equalTaquin(pTaquin, (tmp)->Taq)) return tmp;

		tmp = (tmp)->next;

	}

	return NULL;

	/* RECURSIF
	if (!&ppHead || !ppHead || !*ppHead) return NULL;

	if (equalTaquin(pTaquin, (*ppHead)->Taq))
		return ppHead;

	return isInList(&((*ppHead)->next),pTaquin);*/
}

// fonction pour afficher une liste
// si on met displayFGH à 0 les valeur de F, G et H ne sont pas affichées
int displayList(ptrListAStar pHead, int displayFGH)
{
	if (!&pHead || !pHead) {
		printf("X\n");
		return 1;
	}

	printf("[ *");
	if (displayFGH) printf(" F=%d G=%d H=%d d=%d", pHead->g + pHead->h, pHead->g, pHead->h, pHead->d);
	printf(" ] -> ");

	return displayList(pHead->next, displayFGH);
}

void freeList(ptrListAStar* list) {

	ptrListAStar* tmp;

	while (*list) {

		freeTaquin((*list)->Taq);

		tmp = *list;
		(*list) = (*list)->next;
		free(tmp);

		//printf("J'ai free !\n");

	}

}

// Fonction pour résoudre le taquin en utilisant l'algorithme A*
// La fonction prend comme taquin initial le taquin spécifié par pTaquin
// elle remplit 
//   - pTabDeplacement (un tableau contenant les déplacements à effectuer pour résoudre le taquin)
//   - pNbDeplacements
//   - pNbTaquinsGeneres
//   - pTimeElapsed
// Si stepByStep est différent de 0 on affiche dans la console toutes les étapes de la résolution
// pWindow
int solveTaquin(Taquin* pTaquin, deplacement** pTabDeplacement, unsigned long* pNbDeplacements, unsigned long* pNbTaquinsGeneres, unsigned long* pTimeElapsed, int stepByStep, SDL_Surface* pWindow)
{
	int t1 = clock();

	ptrListAStar* closed = (ptrListAStar*)calloc(1, sizeof(ptrListAStar));
	ptrListAStar* opened = (ptrListAStar*)calloc(1, sizeof(ptrListAStar));

	ptrListAStar node = createNodeList(pTaquin, 0, h(pTaquin), AUCUN, NULL);
	insertList(closed, node, 0);

	Taquin* tmp = (Taquin*)calloc(1, sizeof(Taquin));
	Taquin* tmp2 = (Taquin*)calloc(1, sizeof(Taquin));

	//Taquin courant issu des mouvements réalisés.
	createTaquin(tmp, pTaquin->hauteur, pTaquin->largeur);
	copyTaquin(pTaquin, tmp);
	//Taquin qui parcours les possibilités selon les mouvements.
	createTaquin(tmp2, pTaquin->hauteur, pTaquin->largeur);

	//Mouvements réalisés.
	int g = 0;
	//nb taquin généré
	int taq = 0;

	while (!endTaquin(tmp)) {

		//Pour chaque mouvements...
		for (int i = 1; i < 5; i++) {

			copyTaquin(tmp, tmp2);
			int depla = 0;

			if (i == 1)depla = 2;
			if (i == 2)depla = 1;
			if (i == 3)depla = 4;
			if (i == 4)depla = 3;

			//Si mouvement possible et plateau non encore exploré...
			if (!((tmp2->y == tmp2->largeur - 1 && i == 1) || (tmp2->y == 0 && i == 2) || (tmp2->x == tmp2->hauteur - 1 && i == 3) || (tmp2->x == 0 && i == 4)) && moveTaquin(tmp2, i) && isInList(closed, tmp2) == NULL && (g == 0 || isInList(opened, tmp2) == NULL)) {
				taq++;
				//On sauvegarde le Taquin dans la liste opened (triée).
				node = createNodeList(tmp2, g + 1, g + 1 + h(tmp2), i, tmp);
				insertList(opened, node, 1);
				//displayList(*opened, 1);

			}

		}

		//On extrait le taquin le plus favorable pour le sauvegarder dans la liste closed...
		node = popList(opened);
		pTabDeplacement[g] = (deplacement*)calloc(1, sizeof(deplacement));
		*pTabDeplacement[g] = node->d;
		insertList(closed, node, 0);
		g++;

		//Et on reprend ce taquin comme base pour les prochaines recherches.
		copyTaquin(node->Taq, tmp);

		if (stepByStep) {
			printf("\nMove : %d | H = %d\n", node->d, node->h);
			moveTaquin(pTaquin, node->d);
			displayTaquin(tmp, 0);
			
		}

	}

	int t2 = clock();

	*pTimeElapsed = t2 - t1;

	*pNbDeplacements = g;
	*pNbTaquinsGeneres = taq;

	displayTaquin(tmp, 0);

	freeList(opened);
	freeList(closed);

	return 1;
}

// fonction d'évaluation pour la résolution avec AStar
int h(Taquin* pTaquin)
{
	int he = 0;
	int ha = pTaquin->hauteur;
	int la = pTaquin->largeur;

	//Distance de Manhattan.
	for (int j = 0; j < ha * la; j++) {
		for (int i = 0; i < ha * la; i++) {

			if (j == pTaquin->plateau[(int)(i / la)][i % la]) {
				he += abs(j % la - i % la) + abs((int)(j / la) - (int)(i / la));
				break;
			}

		}
	}

	return he / 2;
}