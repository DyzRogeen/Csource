#include "Scene.h"
#include "Window.h"
#include "Matrix.h"
#include "Vector.h"
#include "Object.h"
#include "Graphics.h"
#include "Shader.h"

Scene *Scene_New(Window *window)
{
    Scene *scene = NULL;
    int meshCapacity = 16;
    int exitStatus;

    scene = (Scene *)calloc(1, sizeof(Scene));
    if (!scene) goto ERROR_LABEL;

    scene->m_renderer = Window_getRenderer(window);

    scene->m_meshes = (Mesh **)calloc(meshCapacity, sizeof(Mesh *));
    if (!scene->m_meshes) goto ERROR_LABEL;

    scene->m_meshCount = 0;
    scene->m_meshCapacity = meshCapacity;


    // Crée la racine
    Object *root = Scene_CreateObject(scene, sizeof(Object));
    if (!root) goto ERROR_LABEL;

    exitStatus = Object_Init(root, scene, Mat4_Identity, NULL);
    if (exitStatus != EXIT_SUCCESS) goto ERROR_LABEL;

    scene->m_root = root;

    // Crée la caméra
    Mat4 camTransform = Mat4_GetTranslationMatrix(Vec3_Set(0.0f, 0.0f, 5.0f));
    Camera *camera = (Camera *)Scene_CreateObject(scene, sizeof(Camera));
    if (!camera) goto ERROR_LABEL;

    exitStatus = Camera_Init(camera, scene, camTransform, root);
    if (exitStatus != EXIT_SUCCESS) goto ERROR_LABEL;

    scene->m_camera = camera;

    // Définit les lumières
    Scene_SetLightDirection(scene, Vec3_Set(0.5, 1.0, 0.5));
    Scene_SetLightColor(scene, Vec3_Set(0.7f, 0.6f, 0.5f));
    Scene_SetAmbiantColor(scene, Vec3_Set(0.3f, 0.3f, 0.3f));

    // Définit les shaders par défaut
    scene->m_defaultVShader = VertexShader_Base;
    scene->m_defaultFShader = FragmentShader_Base;

    return scene;

ERROR_LABEL:
    printf("ERROR - Scene_new()\n");
    assert(false);
    Scene_Free(scene);
    return NULL;
}

void Scene_Free(Scene *scene)
{
    if (!scene) return;

    // Supprime l'ensemble des objets
    Scene_RemoveObject(scene, scene->m_root);

    // Supprime les meshes
    int meshCount = scene->m_meshCount;
    for (int i = 0; i < meshCount; ++i)
    {
        Mesh_Free(scene->m_meshes[i]);
    }
    free(scene->m_meshes);

    // Met à zéro la mémoire (sécurité)
    memset(scene, 0, sizeof(Scene));

    free(scene);
}

Object *Scene_CreateObject(Scene *scene, int size)
{
    Object *object = NULL;

    assert(size >= sizeof(Object));

    object = (Object *)calloc(1, size);
    if (!object) goto ERROR_LABEL;

    return object;

ERROR_LABEL:
    printf("ERROR - Scene_CreateObject()\n");
    assert(false);
    return NULL;
}

void Scene_RemoveObject(Scene *scene, Object *object)
{
    assert(scene && object);

    while (Object_GetChildCount(object) > 0)
    {
        Object *child = Object_GetFirstChild(object);
        Scene_RemoveObject(scene, child);
    }

    Object_Destroy(object);
    free(object);
}

static int Scene_EnsureMeshCapacity(Scene *scene, int meshCount)
{
    int meshCapacity = scene->m_meshCapacity;
    if (meshCapacity >= meshCount)
    {
        return EXIT_SUCCESS;
    }

    meshCapacity = Int_Max(meshCapacity << 1, meshCount + 1);
    Mesh **newMeshes = (Mesh **)realloc(scene->m_meshes, meshCapacity * sizeof(Mesh *));
    if (!newMeshes) goto ERROR_LABEL;

    scene->m_meshes = newMeshes;
    scene->m_meshCapacity = meshCapacity;

    return EXIT_SUCCESS;

ERROR_LABEL:
    printf("ERROR - Scene_EnsureMeshCapacity()\n");
    assert(false);
    return EXIT_FAILURE;
}

Mesh *Scene_CreateMeshFromOBJ(Scene *scene, char *folderPath, char *fileName)
{
    Mesh *mesh = NULL;
    int meshCount = scene->m_meshCount;
    int meshCapacity = scene->m_meshCapacity;

    int exitStatus = Scene_EnsureMeshCapacity(scene, meshCount + 1);
    if (exitStatus != EXIT_SUCCESS) goto ERROR_LABEL;

    mesh = Mesh_LoadOBJ(folderPath, fileName);
    if (!mesh) goto ERROR_LABEL;

    exitStatus = Mesh_ComputeTangents(mesh);
    if (exitStatus != EXIT_SUCCESS) goto ERROR_LABEL;

    scene->m_meshes[meshCount] = mesh;
    scene->m_meshCount = meshCount + 1;

    return mesh;

ERROR_LABEL:
    printf("ERROR - Scene_CreateMeshFromOBJ()\n");
    assert(false);
    return NULL;
}

Object *Scene_GetRoot(Scene *scene)
{
    return scene->m_root;
}

Camera *Scene_GetCamera(Scene *scene)
{
    return scene->m_camera;
}


void Scene_RenderObjectRec(Scene *scene, Object *object)
{
    int childCount = Object_GetChildCount(object);
    Object **children = Object_GetChildren(object);
    for (int i = 0; i < childCount; ++i)
    {
        Scene_RenderObjectRec(scene, children[i]);
    }

    Renderer *renderer = scene->m_renderer;
    VertexShader *vertShader = scene->m_defaultVShader;
    FragmentShader *fragShader = scene->m_defaultFShader;

    Graphics_RenderObject(renderer, object, vertShader, fragShader);
}

void Scene_Render(Scene *scene)
{
    Vec4 backgroundColor = Vec4_Set(0.08f, 0.08f, 0.12f, 1.0f);

    Renderer_ResetDepthBuffer(scene->m_renderer);
    Renderer_Fill(scene->m_renderer, backgroundColor);
    Scene_RenderObjectRec(scene, Scene_GetRoot(scene));
}
