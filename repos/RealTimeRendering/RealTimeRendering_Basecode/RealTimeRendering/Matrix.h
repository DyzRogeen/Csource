#ifndef _MATRIX_H_
#define _MATRIX_H_

/// @file Matrix.h
/// @defgroup Matrix
/// @{

#include "Settings.h"
#include "Vector.h"

//-------------------------------------------------------------------------------------------------
// Structure Mat3

/// @brief Structure représentant une matrice cartésienne 3x3.
typedef struct Mat3_s
{
    float data[3][3];
} Mat3;

/// @brief Matrice identité 3x3.
extern const Mat3 Mat3_Identity;

/// @brief Calcule le produit de deux matrices 3x3.
/// @param mat1 la première matrice.
/// @param mat2 la seconde matrice.
/// @return Le produit de mat1 par mat2
Mat3 Mat3_MulMM(Mat3 mat1, Mat3 mat2);

/// @brief Calcule le produit d'une matrice 3x3 avec un vecteur de taille 3.
/// @param mat la matrice de la transformation linéaire (exprimée dans la base canonique).
/// @param v le vecteur.
/// @return Le produit matrice-vecteur de mat par v.
Vec3 Mat3_MulMV(Mat3 mat, Vec3 v);

/// @brief Calcule le déterminant d'une matrice 3x3.
/// @param mat la matrice.
/// @return Le déterminant de mat.
float Mat3_Det(Mat3 mat);

//-------------------------------------------------------------------------------------------------
// Structure Mat4

/// @brief Structure représentant une matrice homogène 4x4.
typedef union Mat4_s
{
    Vec4  lines[4];
    float data[4][4];
} Mat4;

/// @brief Matrice identité 4x4.
extern const Mat4 Mat4_Identity;

/// @brief Calcule la transposée d'une matrice 4x4.
/// @param matrix la matrice.
/// @return La transposée de matrix.
Mat4 Mat4_Transpose(Mat4 matrix);

/// @brief Calcule le produit de deux matrices 4x4.
/// @param mat1 la première matrice.
/// @param mat2 la seconde matrice.
/// @return Le produit de mat1 par mat2
Mat4 Mat4_MulMM(Mat4 mat1, Mat4 mat2);

/// @brief Calcule le produit d'une matrice 4x4 avec un vecteur de taille 4.
/// @param mat la matrice de la transformation linéaire (exprimée dans la base canonique).
/// @param v le vecteur.
/// @return Le produit matrice-vecteur de mat par v.
Vec4 Mat4_MulMV(Mat4 mat, Vec4 v);

/// @brief Multiplie les coefficients d'une matrice 4x4 par un scalaire.
/// Cette fonction ne renvoie pas la matrice de transformation homogène correspondant à une homothétie.
/// @param mat la matrice.
/// @param s le scalaire.
/// @return La matrice dont les coefficients sont ceux de mat multipliés par s.
Mat4 Mat4_Scale(Mat4 mat, float s);

/// @brief Calcule le cofacteur d'indice (i,j) d'une matrice.
/// Cela correspond à (-1)^(i+j) * det(M_ij) où M_ij désigne la matrice donnée
/// à laquelle on a enlever la ligne i et la colonne j.
/// @param mat la matrice.
/// @param i indice de ligne (entre 0 et 3 inclus).
/// @param j indice de colonne (entre 0 et 3 inclus).
/// @return le cofacteur d'indice (i,j) de mat.
float Mat4_Cofactor(Mat4 m, int i, int j);

/// @brief Calcule la matrice des cofacteurs.
/// @param mat la matrice.
/// @return La matrice des cofacteurs de mat.
Mat4 Mat4_CofactorMatrix(Mat4 mat);

/// @brief Calcule le déterminant d'une matrice 4x4.
/// @param mat la matrice.
/// @return Le déterminant de mat.
float Mat4_Det(Mat4 m);

/// @brief Calcule l'inverse d'une matrice 4x4.
/// @param mat la matrice.
/// @return L'inverse de mat.
Mat4 Mat4_Inv(Mat4 mat);

//-------------------------------------------------------------------------------------------------
// Transformations géométriques en coordonnées homogènes

/// @brief Renvoie la matrice de la transformation effectuant un changement d'échelle
/// en coordonnées homogènes.
/// @param[in] s le facteur d'échelle à appliquer à tous les axes.
/// @return La matrice de la transformation.
Mat4 Mat4_GetScaleMatrix(float s);

/// @brief Renvoie la matrice de la transformation effectuant un changement d'échelle (pour chaque axe)
/// en coordonnées homogènes.
/// @param[in] v le vecteur donnant les coefficients pour les axes x, y, et z.
/// @return La matrice de la transformation.
Mat4 Mat4_GetScale3Matrix(Vec3 v);

/// @brief Renvoie la matrice de la transformation effectuant une translation en coordonnées homogènes.
/// @param[in] v le vecteur de translation.
/// @return La matrice de la transformation.
Mat4 Mat4_GetTranslationMatrix(Vec3 v);

/// @brief Renvoie la matrice de la transformation effectuant une rotation autour de l'axe des x en
/// coordonnées homogènes.
/// @param[in] angle l'angle de la rotation en degrés.
/// @return La matrice de la transformation.
Mat4 Mat4_GetXRotationMatrix(float angle);

/// @brief Renvoie la matrice de la transformation effectuant une rotation autour de l'axe des y en
/// coordonnées homogènes.
/// @param[in] angle l'angle de la rotation en degrés.
/// @return La matrice de la transformation.
Mat4 Mat4_GetYRotationMatrix(float angle);

/// @brief Renvoie la matrice de la transformation effectuant une rotation autour de l'axe des z en
//  coordonnées homogènes.
/// @param angle l'angle de la rotation en degrés.
/// @return La matrice de la transformation.
Mat4 Mat4_GetZRotationMatrix(float angle);

/// @brief Renvoie la matrice de la transformation effectuant une projection en perspective en
/// coordonnées homogènes.
/// @param[in] l la coordonée de la gauche du frustum.
/// @param[in] r la coordonée de la droite du frustum. 
/// @param[in] b la coordonée du bas du frustum.
/// @param[in] t la coordonée du haut du frustum.
/// @param[in] n la coordonée de l'avant du frustum.
/// @param[in] f la coordonée de l'arrière du frustum.
/// @return La matrice de la transformation.
Mat4 Mat4_GetProjectionMatrix(float l, float r, float b, float t, float n, float f);

/// @}
#endif