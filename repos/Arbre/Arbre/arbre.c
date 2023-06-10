#include "arbre.h"
#include <stdlib.h>
#include <stdio.h>

treeNode* creerNoeudArbre(int p_value) {

	treeNode* tree = (treeNode*)calloc(1,sizeof(treeNode));
	tree->m_data = p_value;
	return tree;

}

void libererArbre(treeNode** p_pRoot) {

	if (!(*p_pRoot))return;
	if ((*p_pRoot)->tab[GAUCHE] == NULL && (*p_pRoot)->tab[DROITE] == NULL) {
		free(*p_pRoot);
		return;
	}

	libererArbre(&((*p_pRoot)->tab[GAUCHE]));
	libererArbre(&((*p_pRoot)->tab[DROITE]));
	return;

}

void ajoutGauche(treeNode** p_pRoot, int p_value) {

	if (!p_pRoot || !*p_pRoot)return;
	if ((*p_pRoot)->tab[GAUCHE] = NULL) (*p_pRoot)->tab[GAUCHE] = creerNoeudArbre(p_value);
	ajoutGauche((*p_pRoot)->tab[GAUCHE], p_value);

}

void ajoutDroite(treeNode** p_pRoot, int p_value) {

	if (!p_pRoot || !*p_pRoot)return;
	if ((*p_pRoot)->tab[DROITE] = NULL) (*p_pRoot)->tab[DROITE] = creerNoeudArbre(p_value);
	ajoutGauche((*p_pRoot)->tab[DROITE], p_value);

}

int hauteur(treeNode* p_pRoot) {

	if (!p_pRoot)return -1;
	int nl, nr;
	nl = hauteur(p_pRoot->tab[GAUCHE]);
	nr = hauteur(p_pRoot->tab[DROITE]);
	return (nl > nr) ? nl + 1 : nr + 1;

}

int taille(treeNode* p_pRoot) {

	if (!p_pRoot)return 0;
	return 1 + taille(p_pRoot->tab[GAUCHE]) + taille(p_pRoot->tab[DROITE]);

}

int nbFeuilles(treeNode* p_pRoot) {

	if (!p_pRoot->tab[GAUCHE] && !p_pRoot->tab[DROITE])return 1;
	return nbFeuilles(p_pRoot->tab[GAUCHE]) + nbFeuilles(p_pRoot->tab[DROITE]);

}

void supprSousArbre(treeNode** p_pRoot, int p_value) {

	if (!*p_pRoot) return;
	if ((*p_pRoot)->m_data == p_value) libererArbre(p_pRoot);

	supprSousArbre(&((*p_pRoot)->tab[GAUCHE]),p_value);
	supprSousArbre(&((*p_pRoot)->tab[DROITE]),p_value);

}

void parcoursPrefixe(treeNode* p_pRoot) {

	if (!p_pRoot)return;
	printf("Val = %d\n", p_pRoot->m_data);
	parcoursPrefixe(p_pRoot->tab[GAUCHE]);
	parcoursPrefixe(p_pRoot->tab[DROITE]);

}

void parcoursInfixe(treeNode* p_pRoot) {

	if (!p_pRoot)return;
	parcoursInfixe(p_pRoot->tab[GAUCHE]);
	parcoursInfixe(p_pRoot->tab[DROITE]);
	printf("Val = %d\n", p_pRoot->m_data);

}

void parcoursPostfixe(treeNode* p_pRoot) {

	if (!p_pRoot)return;
	parcoursPostfixe(p_pRoot->tab[GAUCHE]);
	printf("Val = %d\n", p_pRoot->m_data);
	parcoursPostfixe(p_pRoot->tab[DROITE]);

}