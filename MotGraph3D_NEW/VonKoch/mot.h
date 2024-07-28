#pragma once

#include "utils_SDL.h"
#define PI	3.14159265359

typedef struct sPoint {
	float x;
	float y;
	int depth;
	int display;
}point;

typedef struct sPoint3 {
	float x,y,z;
	point p;
	struct sPoint3* next;
}point3;

typedef struct sListP {
	point3* p;
	struct sListP* next;
}listP;

typedef struct sFace {
	int nbPoints;
	point3 normale;
	point3 G;
	listP* points;
	Uint32 color;
	int display;
	struct sFace* next;
}face;

typedef struct sObj3 {
	int nbFaces, nbVertexes;
	face* faces;
	point3* vertexes;
	point3 G;
	// TODO
	int isStatic;
	point3 velocity;
	float mass;
}obj;

typedef struct sSphere {
	Uint32 color;
	float radius;
	point3* pos;
	point3 v;
}sphere;

typedef struct sListS {
	sphere* s;
	struct sListS* next;
}listS;

typedef struct sListO {
	obj* o;
	struct sListO* next;
}listO;

typedef struct sCam {
	point3 pos, pCam;
	point3 dir, vUnit1, vUnit2;
	float lon, lat;
	float dist;
}cam;

typedef struct sLight {
	Uint32 color;
	float intensity;
	point3* pos;
}light;

typedef struct sListL {
	light* l;
	struct sListL* next;
}listL;

// Utils
void printP3(point3 p);
int isInPlane(face f, point3* p);
int checkFaceDisplay(face f, cam c);
point3 getUnitV(point3 v);
listP* createListP(point3* p);
void printFace(face f);

// Vector Basic Operations

// 2D
point sum2(point p1, point p2, int diff);
float norm2(point p);
point scale2(point p, float n);
point getUnitV2(point p);

// 3D
point3 sum(point3 p1, point3 p2, int diff);
void add(point3* p1, point3 p2, int diff);
point3 scale(point3 p, float n);
float scalar(point3 p1, point3 p2);
float norm(point3 p);

// Points methods
point3* createPoint(float x, float y, float z);
point3 setPoint(float x, float y, float z);

// Faces methods
face* createFace(Uint32 color, int nbPoints, ...);
void addPointFace(face *f, point3* p);

// Spheres methods
void addSphere(listS** S, sphere* s);
listS* createListS(sphere* s);
sphere* createSphere(float radius, point3* pos, Uint32 color);

// Objects methods
obj* createObj3D(face* f, point3* p, int nbFaces, int nbVertexes, int isStatic);
obj* createCube(point3 pos, point3 rot, float size, Uint32 color, int isStatic);
void addObj(listO** O, obj* o);

// Graphic methods

// Camera
cam initCam(point3 pos, point3 dir);
void moveCam(cam* C, point3 pos);
void rotateCam(cam* C, float lon, float lat);

// Light
void addLight(listL** L, light* l);
listL* createListL(light* l);
light* createLight(point3* pos, float intensity, Uint32 color);
void applyLights(face f, cam c, listL* L, Uint8 color[3]);

// Render
void pointsTo2dProjection(int screenW, int screenH, point3* p, cam c);
void drawSpheres(SDL_Surface* window, int* Z_Buffers, listS* S, listL* L, cam c, int alt);
void drawLine(SDL_Surface* window, point p1, point p2, Uint32 color);
void displayLights(SDL_Surface* window, int* Z_buffers, listL* L, cam c, int alt);
void displayFloorLines(SDL_Surface* window, int* Z_Buffers, cam c);
void displayObj(SDL_Surface* window, int* Z_Buffers, obj o, cam c, listL* L, int camHasMoved, int alt);
void colorRow(SDL_Surface* window, int* Z_Buffers, int x1, int x2, int y, int z1, int z2, const Uint32 color, int alt);
void colorTriangle(SDL_Surface* p_affichage, int* Z_Buffers, point p1, point p2, point p3, const Uint32 color, int alt);
void colorFace(SDL_Surface* window, int* Z_Buffers, listP* p, int nbPoints, const Uint32 color, int alt);

// Modelisation Methods
void RotateObj(obj* o, point3 rot);
void ExtrudeFace(obj* o, point3 dir);

// Physics Methods
void moveSpheres(listS* S);

// Memory Cleaning
void freeAll(listO* O, listS* S, listL* L);
void freeListO(listO* O);
void freeObj(obj* o);
void freeFaces(face* f);
void freeListS(listS* O);
void freeSphere(sphere* s);
void freeListP(listP* P);
void freePoints(point3* p);
void freeListL(listL* O);
void freeLight(light* l);

/*TODO
* 
* save face reflection color in other slot in face structure to avoid repeating addLight
* trucs à faire dans drawSpheres
* collision avec spheres
* 
*/