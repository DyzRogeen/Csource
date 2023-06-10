#include "Matrix.h"
#include "Vector.h"
#include "Tools.h"

const Mat3 Mat3_Identity = {
    1.0f, 0.0f, 0.0f,
    0.0f, 1.0f, 0.0f,
    0.0f, 0.0f, 1.0f
};
const Mat4 Mat4_Identity = {
    1.0f, 0.0f, 0.0f, 0.0f,
    0.0f, 1.0f, 0.0f, 0.0f,
    0.0f, 0.0f, 1.0f, 0.0f,
    0.0f, 0.0f, 0.0f, 1.0f
};

//-------------------------------------------------------------------------------------------------
// Fonctions à coder

// TODO
Mat3 Mat3_MulMM(Mat3 mat1, Mat3 mat2)
{
    Mat3 m;
    for (int l = 0; l < 3; l++) {
        for (int c = 0; c < 3; c++) {
            m.data[l][c] = 0;
            for (int i = 0; i < 3; i++) {
                m.data[l][c] += mat1.data[l][i] * mat2.data[i][c];
            }
        }
    }
    return m;
}

Vec3 Mat3_MulMV(Mat3 mat, Vec3 v)
{
    Vec3 u;
    u.x = mat.data[0][0] * v.x + mat.data[0][1] * v.y + mat.data[0][2] * v.z;
    u.y = mat.data[1][0] * v.x + mat.data[1][1] * v.y + mat.data[1][2] * v.z;
    u.z = mat.data[2][0] * v.x + mat.data[2][1] * v.y + mat.data[2][2] * v.z;
    return u;
}

float Mat3_Det(Mat3 mat)
{
    float det = 0.0f;
    float prod = 1;
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            prod *= mat.data[j][(i + j) % 3];
        }
        det += prod;
        prod = 1;
    }
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            prod *= mat.data[2 - j][(i + j) % 3];
        }
        det -= prod;
        prod = 1;
    }
    return det;
}

Mat4 Mat4_Transpose(Mat4 matrix)
{
    Mat4 m;
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            m.data[i][j] = matrix.data[j][i];
        }
    }return m;
}

Mat4 Mat4_MulMM(Mat4 mat1, Mat4 mat2)
{
    Mat4 m;
    for (int l = 0; l < 4; l++) {
        for (int c = 0; c < 4; c++) {
            m.data[l][c] = 0;
            for (int i = 0; i < 4; i++) {
                m.data[l][c] += mat1.data[l][i] * mat2.data[i][c];
            }
        }
    }
    return m;
}

Vec4 Mat4_MulMV(Mat4 mat, Vec4 v)
{
    Vec4 u;
    u.x = mat.data[0][0] * v.x + mat.data[0][1] * v.y + mat.data[0][2] * v.z + mat.data[0][3] * v.w;
    u.y = mat.data[1][0] * v.x + mat.data[1][1] * v.y + mat.data[1][2] * v.z + mat.data[1][3] * v.w;
    u.z = mat.data[2][0] * v.x + mat.data[2][1] * v.y + mat.data[2][2] * v.z + mat.data[2][3] * v.w;
    u.w = mat.data[3][0] * v.x + mat.data[3][1] * v.y + mat.data[3][2] * v.z + mat.data[3][3] * v.w;
    return u;
}

Mat4 Mat4_Scale(Mat4 mat, float s)
{
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            mat.data[i][j] *= s;
        }
    }return mat;
}

float Mat4_Cofactor(Mat4 m, int i, int j)
{
    Mat3 mat;
    for (int k = 0; k < 3; k++) {
        for (int l = 0; l < 3; l++) {
            mat.data[k][l] = m.data[k + (k >= i)][l + (l >= j)];
        }
    }
    float det = mat.data[0][0] * mat.data[1][1] * mat.data[2][2] + mat.data[0][1] * mat.data[1][2] * mat.data[2][0] + mat.data[0][2] * mat.data[1][0] * mat.data[2][1] - mat.data[0][2] * mat.data[1][1] * mat.data[2][0] - mat.data[0][1] * mat.data[1][0] * mat.data[2][2] - mat.data[0][0] * mat.data[1][2] * mat.data[2][1];
    return pow(-1, i + j) * det;

}

Mat4 Mat4_CofactorMatrix(Mat4 mat)
{
    Mat4 m;
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            m.data[i][j] = Mat4_Cofactor(mat, i, j);
        }
    }return m;
}


float Mat4_Det(Mat4 m)
{
    float det4 = 0;
    Mat3 mat;
    for (int n = 0; n < 4; n++) {
        for (int k = 0; k < 3; k++) {
            for (int l = 0; l < 3; l++) {
                mat.data[k][l] = m.data[k + 1][l + (l >= n)];
            }
        }
        det4 += Mat3_Det(mat) * pow(-1, n) * m.data[0][n];
    }
    return det4;
}

Mat4 Mat4_Inv(Mat4 mat)
{
    float det4 = Mat4_Det(mat);
    Mat4 m = { 1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1 };
    if (det4 != 0) {
        m = Mat4_Scale(Mat4_Transpose(Mat4_CofactorMatrix(mat)), 1/det4);
    }
    return m;
}

//-------------------------------------------------------------------------------------------------
// Fonctions du basecode

Mat4 Mat4_GetProjectionMatrix(float l, float r, float b, float t, float n, float f)
{
    Mat4 projectionMatrix = Mat4_Identity;

    if (r == l || t == b || f == n)
        return projectionMatrix;

    n = -n;
    f = -f;

    projectionMatrix.data[0][0] = 2.f * n / (r - l);
    projectionMatrix.data[1][1] = 2.f * n / (t - b);
    projectionMatrix.data[2][2] = (f + n) / (f - n);
    projectionMatrix.data[3][3] = 0.f;

    projectionMatrix.data[0][2] = -(r + l) / (r - l);
    projectionMatrix.data[1][2] = -(t + b) / (t - b);
    projectionMatrix.data[3][2] = 1.f;

    projectionMatrix.data[2][3] = -2.f * f * n / (f - n);

    return projectionMatrix;
}


Mat4 Mat4_GetScaleMatrix(float s)
{
    Mat4 scaleMatrix = Mat4_Identity;

    for (int i = 0; i < 3; i++)
        scaleMatrix.data[i][i] = s;

    return scaleMatrix;
}

Mat4 Mat4_GetScale3Matrix(Vec3 v)
{
    Mat4 scaleMatrix = Mat4_Identity;

    for (int i = 0; i < 3; i++)
        scaleMatrix.data[i][i] = v.data[i];

    return scaleMatrix;
}

Mat4 Mat4_GetTranslationMatrix(Vec3 v)
{
    Mat4 translationMatrix = Mat4_Identity;

    for (int i = 0; i < 3; i++)
    {
        translationMatrix.data[i][3] = v.data[i];
    }
    translationMatrix.data[3][3] = 1.f;

    return translationMatrix;
}

Mat4 Mat4_GetXRotationMatrix(float angle)
{
    Mat4 rotationMatrix = Mat4_Identity;

    angle = (float)(angle * M_PI / 180.f);
    float c = cosf(angle);
    float s = sinf(angle);

    rotationMatrix.data[1][1] = c;
    rotationMatrix.data[2][2] = c;

    rotationMatrix.data[1][2] = -s;
    rotationMatrix.data[2][1] = s;

    return rotationMatrix;
}

Mat4 Mat4_GetYRotationMatrix(float angle)
{
    Mat4 rotationMatrix = Mat4_Identity;

    angle = (float)(angle * M_PI / 180.f);
    float c = cosf(angle);
    float s = sinf(angle);

    rotationMatrix.data[0][0] = c;
    rotationMatrix.data[2][2] = c;

    rotationMatrix.data[2][0] = -s;
    rotationMatrix.data[0][2] = s;

    return rotationMatrix;
}

Mat4 Mat4_GetZRotationMatrix(float angle)
{
    Mat4 rotationMatrix = Mat4_Identity;

    angle = (float)(angle * M_PI / 180.f);
    float c = cosf(angle);
    float s = sinf(angle);

    rotationMatrix.data[0][0] = c;
    rotationMatrix.data[1][1] = c;

    rotationMatrix.data[0][1] = -s;
    rotationMatrix.data[1][0] = s;

    return rotationMatrix;
}
