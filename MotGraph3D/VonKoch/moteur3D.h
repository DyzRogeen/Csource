#pragma once

#define PI	3.14159265359

typedef struct sPointS {
	int m_x;
	int m_y;
	int display;
}pointS;

typedef struct sCamera {

	float x, y, z;
	float lon, lat;
	float zoom;

}cam;

typedef struct sPoint {

	float x, y, z;
	struct sPoint* next;
	struct sVoisins* voisins;
	int id;

}point;

typedef struct sVoisins {

	point* p;
	struct sVoisins* vnext;

}vois;

typedef struct sObject {

	point* head;
	int size;
	struct sObject* next;

}obj;

typedef struct sScreen {

	float unit1_x, unit1_y, unit1_z;
	float unit2_x, unit2_y, unit2_z;
	float vect_x, vect_y, vect_z;
	float pos_x, pos_y, pos_z;

}screen;

int inArray(int id, int tab[50]);

cam initCam(float x, float y, float z, float lon, float lat, float zoom);

void addCam(cam* c, float x, float y, float z, float lon, float lat, float zoom);

point* createPoint(float x, float y, float z);

obj* createObject(point* p);

void addObject(obj** head, obj* o);

point* pointById(obj* o, int id);

void addNeighbor(obj* o, int id1, int id2);

void addPoint(obj** o, point* p);

void remPoint(obj* o, int id);

void freePoint(point* p);

void affObject(obj* o);

screen getScreen(cam c);

pointS pointOnScreen(cam c, screen s, point* p);

void displayAll(SDL_Surface* window, cam c, obj* head);