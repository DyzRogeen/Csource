#ifndef _OBJECT_H_
#define _OBJECT_H_

/// @file Object.h
/// @defgroup Object
/// @{

#include "Settings.h"
#include "Matrix.h"
#include "Mesh.h"

typedef struct Scene_s Scene;
typedef struct Object_s Object;

// Object Virtual Method Table
typedef struct ObjectVMT_s
{
    void (*Destroy)(Object *object);
} ObjectVMT;

/// @brief Structure modélisant un objet de la scène.
struct Object_s
{
    Scene   *m_scene;

    /// @brief Matrice de transformation de l'objet par rapport à son parent.
    Mat4     m_localTransform;

    /// @brief Mesh associé à l'objet.
    Mesh    *m_mesh;

    /// @brief Pointeur vers le parent de l'objet. Vaut NULL pour la racine de la scène.
    Object  *m_parent;

    /// @brief Tableau contenant les pointeurs vers les enfants de l'objet.
    Object **m_children;

    /// @brief Nombre d'enfants de l'objet.
    int      m_childCount;

    /// @brief Nombre maximal d'enfants avant une réallocation.
    int      m_childCapacity;

    ObjectVMT *m_vptr;
};

/// @brief Initialise un objet alloué par Scene_CreateObject().
/// @param[in,out] object l'objet à initialiser.
/// @param[in] scene la scène.
/// @param[in] localTransform matrice de transformation locale.
/// @param[in,out] parent Objet parent auquel le nouvel objet sera attaché.
/// @return EXIT_SUCCESS ou EXIT_FAILURE.
int Object_Init(Object *object, Scene *scene, Mat4 localTransform, Object* parent);

/// @brief Fonction appelée par la scène pour libérer la mémoire interne d'un objet.
/// Pour supprimer un objet, il faut appeler Scene_RemoveObject().
/// @param[in,out] object l'objet à détruire.
void Object_Destroy(Object *object);

/// @brief Modifie le parent d'un objet.
/// @param object l'objet dont on souhaite modifier le parent.
/// @param parent nouveau parent.
/// @return EXIT_SUCCESS ou EXIT_FAILURE.
int Object_SetParent(Object* object, Object* parent);

/// @brief Renvoie le parent d'un objet.
/// @param object l'objet dont on souhaite récupérer le parent
/// @return Le parent de l'objet passé en paramètre
Object* Object_GetParent(Object* object);

INLINE Scene *Object_getScene(Object *object)
{
    return object->m_scene;
}

/// @brief Ajoute un mesh à un objet.
/// @param object l'objet sur lequel on souhaite ajouter le mesh.
/// @param mesh mesh à ajouter sur l'objet.
void Object_SetMesh(Object* object, Mesh* mesh);

/// @brief Définit la transformation d'un objet dans un référentiel donné.
/// @param object l'objet auquel on définit la transformation.
/// @param ref le référentiel dans lequel on définit la transformation.
/// @param transform la transformation.
void Object_SetTransform(Object* object, Object* ref, Mat4 transform);

/// @brief Renvoie la matrice de transformation d'un objet dans un référentiel donné.
/// @param object l'objet dont on souhaite obtenir la matrice de transformation.
/// @param ref le référentiel dans lequel est exprimée la transformation.
/// @return La transformation de object dans le référentiel ref.
Mat4 Object_GetTransform(Object* object, Object* ref);

/// @brief Renvoie la matrice de transformation d'un objet dans le référentiel monde.
/// @param object l'objet.
/// @return La matrice de transformation de object dans le référentiel monde.
INLINE Mat4 Object_GetModelMatrix(Object* object)
{
    return Object_GetTransform(object, NULL);
}

/// @brief Renvoie la matrice de transformation inverse d'un objet dans le référentiel monde.
/// @param object l'objet.
/// @return La matrice de transformation inverse de object dans le référentiel monde.
Mat4 Object_GetInvModelMatrix(Object* object);

INLINE Mat4 Object_GetLocalTransform(Object *object)
{
    return object->m_localTransform;
}

INLINE void Object_SetLocalTransform(Object *object, Mat4 localTransform)
{
    object->m_localTransform = localTransform;
}

Vec4 Object_GetPosition(Object *object);

INLINE int Object_GetChildCount(Object *object)
{
    return object->m_childCount;
}

INLINE Object **Object_GetChildren(Object *object)
{
    return object->m_children;
}

INLINE Object *Object_GetFirstChild(Object *object)
{
    return (object->m_childCount > 0) ? object->m_children[0] : NULL;
}
/// @}

#endif