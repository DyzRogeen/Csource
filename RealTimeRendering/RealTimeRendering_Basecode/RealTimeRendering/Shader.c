#include "Shader.h"
#include "Scene.h"
#include "Graphics.h"
#include "Tools.h"

VShaderOut VertexShader_Base(VShaderIn *in, VShaderGlobals *globals)
{
    // Le vertex shader est une fonction exécutée pour chacun des sommets
    // d'un objet à rendre.
    // Il récupère des données globales à l'objet comme les matrices de conversion.
    // Il se charge principalement de projeter le sommet dans le "clip space"
    // et de convertir la normale/tangente associée dans le référentiel monde.
    // Le vertex shader est exécuté pour chacun des sommets d'un objet à rendre.
    VShaderOut out = { 0 };
    
    // Coordonnées homogènes du sommet (w = 1 car c'est un point)
    Vec4 vertex = Vec4_From3(in->vertex, 1.0f);

    //vertex.x /= fabs(sin(Timer_GetElapsed(g_time)))+1.f;
    // Coordonnées homogènes de la normale (w = 0 car c'est une direction)
    Vec4 normal = Vec4_From3(in->normal, 0.0f);

    // Transformation du sommet dans le repère caméra
    Vec4 vertexCamSpace = Mat4_MulMV(globals->objToView, vertex);

    // Projection du sommet dans le "clip space"
    Vec4 vertexClipSpace = Mat4_MulMV(globals->objToClip, vertex); // OBLIGATOIRE (ne pas modifier)

    Vec4 light = Mat4_MulMV(globals->objToView, normal);

    Vec4 posObj = Mat4_MulMV(globals->objToWorld, vertex);

    // TODO
    // Pour la lumière diffuse, il faut transformer la normale dans le repère monde
    // Pour le modèle de Blinn-Phong, il faut calculer la position du sommet
    // dans le référentiel monde et ajouter l'information au VShaderOut

    // Définit la sortie du vertex shader
    out.clipPos = Vec3_From4(vertexClipSpace);  // OBLIGATOIRE (ne pas modifier)
    out.invDepth = vertexCamSpace.w / vertexCamSpace.z; // OBLIGATOIRE (ne pas modifier)
    out.normal = Vec3_Normalize(Vec3_From4(light));
    out.textUV = in->textUV;
    out.worldPos = Vec3_From4(posObj);

    return out;
}

Vec4 FragmentShader_Base(FShaderIn *in, FShaderGlobals *globals)
{
    // Le fragment shader est une fonction exécutée pour chaque pixel
    // de tous les triangles visibles.
    // Il récupère un fragment qui contient des données interpolées
    // (position, normale, tangente, uv).
    // Il retourne la couleur du pixel.
    //
    // Le vecteur (x,y,z,w) renvoyé par le fragment shader est interprété comme suit :
    // - x : intensité du rouge (entre 0 et 1)
    // - y : intensité du vert  (entre 0 et 1)
    // - z : intensité du bleu  (entre 0 et 1)
    // - w : opacité            (entre 0 et 1)

    // Récupération du matériau associé au pixel (albedo/normal map)
    Material *material = globals->material;
    assert(material);

    // Récupération de la texture (albedo) associée au pixel
    MeshTexture *albedoTex = Material_GetAlbedo(material);
    assert(albedoTex);

    // Récupération des coordonnées (u,v) associée au pixel.
    // Les coordonnées (u,v) sont dans [0,1]^2.
    // - u représente l'abscisse du point sur une texture 2D.
    // - v représente l'ordonnée du point sur une texture 2D.
    // (0,0) représente le coin en bas à gauche.
    // (1,1) représente le coin en haut à droite.
    float u = in->textUV.x;
    float v = in->textUV.y;

    // Recupération de la couleur du pixel dans la texture
    Vec3 albedo = MeshTexture_GetColorVec3(albedoTex, Vec2_Set(u, v));

#if 1
    // Récupère les lumières de la scène
    Vec3 lightVector = Scene_GetLight(globals->scene);
    Vec3 lightColor = Scene_GetLightColor(globals->scene);
    Vec3 ambiant = Scene_GetAmbiantColor(globals->scene);

    // Récupère la normale interpolée (non normalisée)
    Vec3 normal = in->normal;

    // Application de la lumière ambiante à l'albedo
    //albedo = Vec3_Mul(albedo, ambiant);

    normal = Vec3_Normalize(normal);
    lightVector = Vec3_Normalize(lightVector);

    float lum = Float_Clamp01(Vec3_Dot(normal, lightVector));

    lightColor = Vec3_Scale(lightColor, lum);

    Vec3 viewVector = Vec3_Normalize(Vec3_Sub(globals->cameraPos, in->worldPos));

    Vec3 halfVector = Vec3_Normalize(Vec3_Add(lightVector, viewVector));

    float brilCoef = 10.f;

    float specCoef = powf(Float_Clamp01(Vec3_Dot(normal, halfVector)), brilCoef);

    lightColor = Vec3_Scale(lightColor, specCoef);

    albedo = Vec3_Mul(albedo, Vec3_Add(ambiant, lightColor));

    // TODO
    // Pour la lumière diffuse, il faut utiliser la normale.
    // Pour la lumière spéculaire de Blinn-Phong, il faut :
    // - récupérer l'interpolation de la position dans le monde du pixel ;
    // - calculer le vecteur de vue, le vecteur moitié
    // Vous devez donc modifier le vertex shader, puis modifier la fonction
    // Graphics_RenderTriangle() pour initialiser l'interpolation puis pour
    // calculer l'interpolation.
    // Utilisez les macros VEC3_INIT_INTERPOLATION() et VEC3_INTERPOLATE().
#endif

    //.............................................................................................
    // Quelques exemples de debug (à décommenter)
    
    // Debug : couleur unique en RGBA (bleu ESIEA)
    //return Vec4_Set(0.21f, 0.66f, 0.88f, 1.0f);
    
    // Debug : coordonnées (uv) de texture
    //return Vec4_From2(in->textUV, 0.0, 1.0f);

    // Debug : normales
    //return Vec4_From3(in->normal, 1.0f);
    
    // Debug : normales V2 
    //return Vec4_From3(Vec3_Scale(Vec3_Add(in->normal, Vec3_One), 0.5f), 1.0f);
    //.............................................................................................

    // Retourne la couleur (albedo) associée au pixel dans la texture.
    return Vec4_From3(albedo, 1.f);

}
