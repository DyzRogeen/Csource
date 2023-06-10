#include "Camera.h"
#include "Matrix.h"
#include "Vector.h"
#include "Window.h"
#include "Graphics.h"

void Camera_VM_Destroy(Object *object);

static ObjectVMT ObjectVMT_Camera = {
    .Destroy = Camera_VM_Destroy
};

int Camera_Init(Camera *camera, Scene *scene, Mat4 localTransform, Object *parent)
{
    assert(camera && scene);

    int exitStatus = Object_Init(
        (Object *)camera, scene, localTransform, parent);
    if (exitStatus != EXIT_SUCCESS) goto ERROR_LABEL;

    ((Object *)camera)->m_vptr = &ObjectVMT_Camera;

    // Valeurs paramètrables :
    // - fov (field of view) : angle de vue
    // - near : profondeur minimale visible
    // - far  : profondeur maximale visible
    float fov = 60.0f;
    float near = 1.0f;
    float far = 50.f;

    float right = tanf((fov / 2.0f) * ((float)M_PI / 180.0f));
    float top = right * 9.0f / 16.0f;

    camera->m_projMatrix = Mat4_GetProjectionMatrix(-right, right, -top, top, near, far);

    return EXIT_SUCCESS;

ERROR_LABEL:
    printf("ERROR - Camera_Init()\n");
    assert(false);
    return EXIT_FAILURE;
}

void Camera_VM_Destroy(Object *object)
{
    // Libère les allocations de la caméra
}

void Camera_SetProjectionMatrix(Camera *camera, Mat4 matrix)
{
    camera->m_projMatrix = matrix;
}
