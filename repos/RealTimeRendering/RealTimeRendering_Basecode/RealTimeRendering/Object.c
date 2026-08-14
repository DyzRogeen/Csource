#include "Object.h"
#include "Matrix.h"
#include "Vector.h"

/// @brief Ajoute un enfant à un objet.
/// Cette fonction est appelée depuis la méthode Object_setParent().
/// @param object l'objet auquel on souhaite ajouter un enfant.
/// @param child l'objet à ajouter comme enfant.
/// @return EXIT_SUCCESS ou EXIT_FAILURE.
int Object_AddChild(Object* object, Object* child);

/// @brief Supprime un enfant à un objet.
/// Cette fonction est appelée depuis la méthode Object_setParent().
/// @param object l'objet auquel on souhaite retirer un enfant.
/// @param child l'objet à retirer de la liste des enfants.
/// @return EXIT_SUCCESS ou EXIT_FAILURE.
int Object_RemoveChild(Object* object, Object* child);

int Object_Init(Object *object, Scene *scene, Mat4 localTransform, Object *parent)
{
    assert(object && scene);
    int capacity = 2;

    object->m_scene = scene;
    object->m_localTransform = localTransform;
    object->m_parent = NULL;
    object->m_mesh = NULL;
    object->m_childCount = 0;
    object->m_childCapacity = capacity;
    object->m_vptr = NULL;

    object->m_children = (Object **)calloc(capacity, sizeof(Object *));
    if (!object->m_children) goto ERROR_LABEL;

    if (parent)
    {
        int exitStatus = Object_SetParent(object, parent);
        if (exitStatus != EXIT_SUCCESS) goto ERROR_LABEL;
    }

    return EXIT_SUCCESS;

ERROR_LABEL:
    printf("ERROR - Object_Init()\n");
    assert(false);
    return EXIT_FAILURE;
}

void Object_Destroy(Object *object)
{
    if (object->m_vptr)
    {
        // Détruit la mémoire allouée par la classe fille
        assert(object->m_vptr->Destroy);
        object->m_vptr->Destroy(object);
    }

    if (object->m_parent)
    {
        // Supprime l'objet des fils du parent
        Object_RemoveChild(object->m_parent, object);
    }

    if (object->m_children)
    {
        free(object->m_children);
    }

    // Met la mémoire à zéro (sécurité)
    memset(object, 0, sizeof(Object));
}

int Object_AddChild(Object* object, Object* child)
{
    if (!object || !child || object == child)
        goto ERROR_LABEL;

    int childCount = object->m_childCount;

    // Vérification que l'objet n'est pas déjà parmi les enfants
    for (int i = 0; i < childCount; i++)
    {
        if (child == object->m_children[i])
            goto ERROR_LABEL;
    }

    // Modification de l'espace nécessaire pour stocker les enfants si nécessaire
    if (childCount >= object->m_childCapacity)
    {
        int capacity = object->m_childCapacity << 1;

        Object **newChildren = (Object **)realloc(object->m_children, capacity * sizeof(Object*));
        if (!newChildren) goto ERROR_LABEL;

        object->m_children = newChildren;
        object->m_childCapacity = capacity;
    }

    // On ajoute child dans la liste des enfants
    object->m_children[childCount++] = child;
    object->m_childCount = childCount;

    return EXIT_SUCCESS;

ERROR_LABEL:
    printf("ERROR - Object_AddChild()\n");
    assert(false);
    return EXIT_FAILURE;
}

int Object_RemoveChild(Object* object, Object* child)
{
    if (!object || !child)
        goto ERROR_LABEL;

    int childCount = object->m_childCount;
    bool found = false;

    for (int i = 0; i < childCount; i++)
    {
        if (object->m_children[i] == child)
        {
            found = true;
            for (int j = i; j < childCount - 1; j++)
            {
                object->m_children[j] = object->m_children[j + 1];
            }
            object->m_children[childCount - 1] = NULL;
            childCount--;
            break;
        }
    }

    if (!found) goto ERROR_LABEL;

    object->m_childCount = childCount;

    return EXIT_SUCCESS;

ERROR_LABEL:
    printf("ERROR - Object_RemoveChild()\n");
    assert(false);
    return EXIT_FAILURE;
}

int Object_SetParent(Object* object, Object* parent)
{
    if (!object) goto ERROR_LABEL;

    // On vérifie que parent ne soit pas l'objet ni qu'il soit déjà son parent
    if (object == parent || object->m_parent == parent) goto ERROR_LABEL;

    //TODO: Modification de la matrice A TESTER
    object->m_localTransform = Object_GetTransform(object, parent);

    //Retrait du noeud object de la liste des enfants de son ancien parent
    if (object->m_parent)
    {
        int exitStatus = Object_RemoveChild(object->m_parent, object);
        if (exitStatus == EXIT_FAILURE) goto ERROR_LABEL;
    }

    // Ajout du parent
    object->m_parent = parent;

    // On ajoute object dans la liste des enfants de parent
    int exitStatus = Object_AddChild(parent, object);
    if (exitStatus == EXIT_FAILURE) goto ERROR_LABEL;

    return EXIT_SUCCESS;

ERROR_LABEL:
    printf("ERROR - Object_setParent()\n");
    assert(false);
    return EXIT_FAILURE;
}

Object* Object_GetParent(Object* object)
{
    if (!object)
        return NULL;

    return object->m_parent;
}

void Object_SetMesh(Object* object, Mesh* mesh)
{
    object->m_mesh = mesh;
}

void Object_SetTransform(Object* object, Object* ref, Mat4 transform)
{
    if (!object)
        return;

    // Si on ne change pas de référentiel, on applique la nouvelle matrice à la matrice locale
    if (ref == object->m_parent)
    {
        object->m_localTransform = transform;
    }
    else
    {
        // Sinon, on trouve la matrice de passage de notre référentiel vers le nouveau
        Mat4 refModelMatrix = Object_GetModelMatrix(ref);
        Mat4 parentModelMatrix = Object_GetInvModelMatrix(object->m_parent);

        object->m_localTransform = Mat4_MulMM(
            Mat4_MulMM(parentModelMatrix, refModelMatrix),
            transform
        );
    }
}

Mat4 Object_GetTransform(Object* object, Object* ref)
{
    Mat4 matrix = Mat4_Identity;

    if (!object || object == ref)
        return matrix;

    // On récupère la matrice de transformation locale
    matrix = object->m_localTransform;
    object = object->m_parent;

    // On parcourt les parents jusqu'à obtenir NULL (le référenciel monde)
    // ou le référentiel voulu
    while (object && object != ref)
    {
        matrix = Mat4_MulMM(object->m_localTransform, matrix);
        object = object->m_parent;
    }

    // Si on atteint le référenciel désiré, on renvoie la matrice
    if (object == ref)
        return matrix;

    // Sinon, on calcule la matrice modèle inverse de l'objet de référence
    // que l'on multiplie à notre matrice
    return Mat4_MulMM(Object_GetInvModelMatrix(ref), matrix);
}

Mat4 Object_GetInvModelMatrix(Object* object)
{
    Mat4 result = Object_GetModelMatrix(object);
    return Mat4_Inv(result);
}

Vec4 Object_GetPosition(Object *object)
{
    Mat4 modelMatrix = Object_GetModelMatrix(object);
    return Mat4_MulMV(modelMatrix, Vec4_ZeroH);
}