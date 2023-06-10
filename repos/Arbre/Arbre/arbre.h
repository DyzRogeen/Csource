#pragma once

typedef struct sTreeNode {

	void* m_data;
	struct sTreeNode* tab[2];

}treeNode;

enum{GAUCHE,DROITE};

treeNode* creerNoeudArbre(int p_value);

void libererArbre(treeNode** p_pRoot);

void ajoutGauche(treeNode** p_pRoot, int p_value);

void ajoutDroite(treeNode** p_pRoot, int p_value);

int hauteur(treeNode* p_pRoot);

int taille(treeNode* p_pRoot);

int nbFeuilles(treeNode* p_pRoot);

void supprSousArbre(treeNode** p_pRoot, int p_value);

void parcoursPrefixe(treeNode* p_pRoot);

void parcoursInfixe(treeNode* p_pRoot);

void parcoursPostfixe(treeNode* p_pRoot);