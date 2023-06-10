#include "arbre.h"
#include <stdlib.h>
#include <stdio.h>

void initTree(treeNode** p_pRoot, int data) {

	if (!(*p_pRoot)) return;
	if (data == 3) return;
	ajoutGauche((*p_pRoot)->tab[GAUCHE],data + 1);
	ajoutDroite((*p_pRoot)->tab[DROITE],data + 2);
	initTree(&((*p_pRoot)->tab[GAUCHE]), data + 2);
	initTree(&((*p_pRoot)->tab[DROITE]), data + 2);

}

int main() {

	treeNode* tree = creerNoeudArbre(1);
	//initTree(&tree, 1);
	tree->tab[GAUCHE] = creerNoeudArbre(2);
	tree->tab[DROITE] = creerNoeudArbre(3);
	ajoutGauche(&tree->tab[GAUCHE], 5);
	ajoutDroite(&tree->tab[DROITE], 7);
	parcoursInfixe(tree);
	printf("Hauteur = %d\nTaille = %d\nnbFeuilles = %d", hauteur(tree), taille(tree), nbFeuilles(tree));

}