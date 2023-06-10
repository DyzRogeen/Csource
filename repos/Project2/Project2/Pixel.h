#pragma once

typedef struct sTreeNode {

	void* m_data;
	struct sTreeNode* tab[4];

}treeNode;

treeNode* creerNoeudArbre(int p_value);

void libererArbre(treeNode** p_pRoot);

typedef unsigned int dword;
typedef unsigned int word;

// Structure d'entête de fichier
typedef struct BMPFILEHEADER {
	char type[2]; // type de fichier, BM pour un fichier BMP
	dword size; // taille du fichier en octets
	dword reserved; // inutilisé
	dword offset; // Décalage des données de l'image par rapport
	// au début du fichier (en octets)
} bmpFileHeader;
// structure d'entête de BMP
typedef struct BMPINFOHEADER {
	dword headersize; // taille de la structure : 40 octets pour un fichier BMP
	dword width; // largeur de l'image
	dword height; // hauteur de l'image
	word planes; // nombre de couches : 1 pour un BMP
	word bitsperpixel; // profondeur des couleurs (nombre de bit par pixel) :
	// 24 dans notre cas
	dword compression; // compression : 0 dans notre cas, pas de compression
	dword sizeimage; // taille de l'image : 0 ou taille de l'image en octets
	// (size - 54), cela dépend des logiciels utilisés
	dword xpixelspermeter; // Résolution d'impression suivant la largeur
	dword ypixelspermeter; // Résoltuion d'impression suivant la hauteur
	dword colorsused; // nombre de couleurs dans la palette :
	// 0 dans notre cas (pas de palette)
	dword colorsimportant; // 0
} bmpInfoHeader;
