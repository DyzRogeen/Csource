#include "Pixel.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#pragma warning(disable:4996)

void addNode(treeNode** p_Root, int index) {

    if (!p_Root || !*p_Root) return;

    (*p_Root)->tab[index] = creerNoeudArbre(-1);

    addNode(&((*p_Root)->m_data), index);

}

int main() {

    int bmpsize = 64;

    treeNode* tree = creerNoeudArbre(1);

    for (int i = 0; i < log(bmpsize) / log(4); i++) {

    }


    /*
    bmpFileHeader* bmpFH = NULL;
    bmpInfoHeader* bmpIH = NULL;

    char* test = "test.bmp";
    FILE* file = fopen(test, "r");
    if (!file)
    {
        printf("Le fichier '%s' n'a pas pu être ouvert.\n", test);
        return 1;
    }
    
    fread(bmpIH, sizeof file, 1, file);

    printf("Largeur = %d\n", bmpIH->width);
    printf("Hauteur = %d\n", bmpIH->height);
    printf("Taille = %d\n", bmpIH->sizeimage);
    printf("bppixel = %d\n", bmpIH->bitsperpixel);

	treeNode* pixel = creerNoeudArbre(0);

    fclose(file);
    */
}