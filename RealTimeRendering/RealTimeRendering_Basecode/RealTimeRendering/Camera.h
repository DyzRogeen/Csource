#ifndef _CAMERA_H_
#define _CAMERA_H_

/// @file Camera.h
/// @defgroup Camera
/// @{

#include "Settings.h"
#include "Object.h"

/// @brief Structure représentant la caméra par laquelle est vue la scène.
typedef struct Camera_s
{
    Object m_base;
    Mat4   m_projMatrix;
} Camera;

int Camera_Init(Camera *camera, Scene *scene, Mat4 localTransform, Object* parent);

/// @brief Définit la matrice de projection utilisé par la caméra.
/// @param[in] camera la caméra.
/// @param[in] matrix la matrice de projection.
void Camera_SetProjectionMatrix(Camera *camera, Mat4 matrix);

/// @}

#endif