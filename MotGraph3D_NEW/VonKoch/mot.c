#include "mot.h"
#include "utils_SDL.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

void printP3(point3 p) {
	printf("[%.1f , %.1f , %.1f]", p.x, p.y, p.z);
}

int isInPlane(face f, point3* p) {

	//Vecteur orthonormal à la normale ?
	point3 v = sum(*f.points, *p, -1);
	point3 n = f.normale;
	return !(n.x * v.x + n.y * v.y + n.z * v.z);

}

///////////////////////////////////////////////////////////

point3 sum(point3 p1, point3 p2, int diff) {

	point3 s;

	if (diff != -1) diff = 1;

	s.x = p1.x + p2.x * diff;
	s.y = p1.y + p2.y * diff;
	s.z = p1.z + p2.z * diff;
	
	return s;
}

void add(point3* p1, point3 p2, int diff) {

	if (diff != -1) diff = 1;

	p1->x += p2.x * diff;
	p1->y += p2.y * diff;
	p1->z += p2.z * diff;
}

point3 scale(point3 p, float n) {

	p.x *= n;
	p.y *= n;
	p.z *= n;
	return p;

}

float norm(point3 p) {
	return sqrtf(p.x * p.x + p.y * p.y + p.z * p.z);
}

///////////////////////////////////////////////////////////

point3* createPoint(float x, float y, float z) {

	point3* p = (point3*)calloc(1, sizeof(point3));
	p->x = x;
	p->y = y;
	p->z = z;
	p->next = NULL;
	
	return p;
}

point3 setPoint(float x, float y, float z) {

	point3 p;
	p.x = x;
	p.y = y;
	p.z = z;

	return p;
}

///////////////////////////////////////////////////////////

edge* createEdge(point3* p1, point3* p2) {
	edge* e = (edge*)calloc(1,sizeof(edge));
	e->points[0] = p1;
	e->points[1] = p2;
	e->next = NULL;

	return e;
}

//PB : normale dynamique à faire
face createFace(point3 n, point3* p, int nbPoints) {

	face f;
	f.points = p;
	f.normale = n;
	f.nbPoints = nbPoints;
	f.next = NULL;
	
	return f;

}

void printFace(face f) {
	printP3(f.normale);
	printf(" : ");
	point3* p = f.points;
	for (int i = 0; i < f.nbPoints; i++) {

		printP3(*p);
		printf(" -> ");
		p = p->next;
	}
	printf("\n");
}

void addPointFace(face *f, point3* p) {

	//if (!isInPlane(f, p))
	//	return f;

	f->nbPoints++;
	//Push
	p->next = f->points;
	f->points = p;

	return f;

}

obj* createObj3D(face* f, edge* e, point3* p, int nbFaces, int nbEdges, int nbVertexes){

	/*int size_f = (int)(sizeof(f) / sizeof(face*));
	int size_e = (int)(sizeof(e) / sizeof(edge*));

	for (int i = size_f - 1; i > 0; i--) f[i]->next = f[i - 1];
	for (int i = size_e - 1; i > 0; i--) e[i]->next = e[i - 1];*/

	obj* o = (obj*)calloc(1, sizeof(obj));

	o->nbFaces = nbFaces;
	o->nbEdges = nbEdges;
	o->nbVertexes = nbVertexes;

	o->faces = f;
	o->edges = e;
	o->vertexes = p;

	return o;

}

///////////////////////////////////////////////////////////

cam initCam(point3 pos, point3 dir) {

	cam c;
	c.pos = pos;
	c.dir = dir;
	c.dist = 5;
	c.lon = 0;
	c.lat = 0;

	c.pCam = sum(c.pos, scale(c.dir, c.dist), 0);
	c.vUnit1 = setPoint(0, 0, 1);
	c.vUnit2 = setPoint(0, 1, 0);
	/*printf("UNITS : \n");
	printP3(c.dir);
	printf("\n");
	printP3(c.vUnit1);
	printf("\n");
	printP3(c.vUnit2);
	printf("\n");*/
	return c;

}

void moveCam(cam* C, point3 pos) {
	add(&C->pos, pos, 0);
	C->pCam = sum(C->pos, scale(C->dir, C->dist), 0);
}

void rotateCam(cam* C, float lon, float lat) {
	C->lon += lon;
	if (C->lon > PI) C->lon = C->lon - 2*PI;
	else if (C->lon < -PI) C->lon = C->lon + 2*PI;
	C->lat += lat;
	if (C->lat > 0.99*PI / 2) C->lat = 0.99*PI / 2;
	else if (C->lat < - 0.99*PI / 2) C->lat = - 0.99*PI / 2;

	lon = C->lon;
	lat = C->lat;

	float normDir = norm(C->dir);
	C->dir = setPoint(normDir * cos(lat) * cos(lon) , normDir * sin(lat), normDir * cos(lat) * sin(lon));

	point3 dir = C->dir;
	float nNorm = sqrtf(dir.x * dir.x + dir.y * dir.y) / normDir;
		
	C->vUnit1 = setPoint(-normDir * sin(lon), 0, normDir * cos(lon));
	C->vUnit2 = setPoint(-normDir * sin(lat) * cos(lon), normDir * cos(lat), -normDir * sin(lat) * sin(lon));

	C->pCam = sum(C->pos, scale(C->dir, C->dist), 0);

	/*printf("lon:%.2f lat: %.2f : ", C->lon, C->lat);
	printf("UNITS : \n");
	printP3(C->dir);
	printf("\n");
	printP3(C->vUnit1);
	printf("\n");
	printP3(C->vUnit2);
	printf("\n");*/
}

void pointsTo2dProjection(point3* p, int nbPoints, cam c) {

	float normDir = norm(c.dir);

	// ref point on the cam plane
	point3 pcam = c.pCam;

	// i and j unit vectors on the cam plane
	point3 vUnit1 = c.vUnit1;
	point3 vUnit2 = c.vUnit2;

	// (X - pcamX)*dirX + (Y - pcamY)*dirY + (Z - pcamZ)*dirZ = 0
	// Cartesian d value in [ ax + by + cz = -d ] for the cam plane
	float d = (pcam.x * c.dir.x) + (pcam.y * c.dir.y) + (pcam.z * c.dir.z);
	float n,k;

	// Intersection coordinates
	float X, Y, Z;

	for (int i = 0; i < nbPoints; i++) {

		// v is vector from point to cam (p to c) we want k such that v*k + p is a point of cam plane
		// so that [ ax + by + cz = -d ] <=> [ a*(vx*k + px) + b*(vy*k + py) + c*(vz*k + pz) = -d ] <=> [ k = -(d + a*px + b*py + c*pz)/(a*vx + b*vy + c*vz) ] with denom != 0
		// if k > 1, point is behind cam plane so must not be visible
		// if k < 0, point is between cam and plane.

		n = c.dir.x * (c.pos.x - p->x) + c.dir.y * (c.pos.y - p->y) + c.dir.z * (c.pos.z - p->z);

		if (n == 0) {
			p->p.display = 0;
			p = p->next;
			continue;
		}

		k = (d - c.dir.x * p->x - c.dir.y * p->y - c.dir.z * p->z) / n;

		if (k > 1 || k < 0) {
			p->p.display = 0;
			p = p->next;
			continue;
		}

		// P = v*k + p
		X = (c.pos.x - p->x) * k + p->x - pcam.x;
		Y = (c.pos.y - p->y) * k + p->y - pcam.y;
		Z = (c.pos.z - p->z) * k + p->z - pcam.z;

		/*
		* x*i + y*j = (X ; Y ; Z) => pcamX + x*v1.x + y*v2.x = X | pcamY + x*v1.y + y*v2.y = Y | pcamZ + x*v1.z + y*v2.z = Z
		* 
		* x*(v1.x - v2.x/v2.y) = X - pcamX - (Y - pcamY)*v2.x/v2.y
		* with r = v2.x/v2.y
		* x = (X - pcamX - (Y - pcamY)*r) / (v1.x - r)
		* y = (Y - pcamY - x*v1.x) / v2.y
		* 
		[        Cam PLANE       ]
		[            ^ v2        ] > and ^ are the i and j unit vectors of the plane
		[            |           ]
		[   , - - - -+---> v1    ] + is the origin reference (pcam)
		[   '        '           ]
		[   ,        ,           ] x is the point intersection projected on the plane
		[   x - - - -'           ] here x is (-2.25 ; -1.5)
		*/
		float den;
		if (fabs(den = vUnit1.x * vUnit2.y - vUnit1.y * vUnit2.x) > 0.00001) {
			p->p.x = round((vUnit2.y * X - vUnit2.x * Y) / den) * normDir;
			p->p.y = -round((vUnit1.y * X - vUnit1.x * Y) / den) * normDir;
		}
		else if (fabs(den = vUnit1.x * vUnit2.z - vUnit1.z * vUnit2.x) > 0.00001) {
			p->p.x = round((vUnit2.z * X - vUnit2.x * Z) / den) * normDir;
			p->p.y = -round((vUnit1.z * X - vUnit1.x * Z) / den) * normDir;
		}
		else {
			den = vUnit1.y * vUnit2.z - vUnit1.z * vUnit2.y;
			p->p.x = round((vUnit2.z * Y - vUnit2.y * Z) / den) * normDir;
			p->p.y = -round((vUnit1.z * Y - vUnit1.y * Z) / den) * normDir;
		} 

		//printf("[%.1f ; %.1f ; %.1f] -> (%d;%d)\n", X, Y, Z, p->p.x, p->p.y);
		p->p.display = 1;
		p = p->next;
	}

}

void displayObj(SDL_Surface* window, obj o, cam c) {

	pointsTo2dProjection(o.vertexes, o.nbVertexes, c);

	edge* e = o.edges;
	point p1, p2;

	for (int i = 0; i < o.nbEdges; i++) {
		p1 = e->points[0]->p;
		p2 = e->points[1]->p;
		if (p1.display && p2.display) _SDL_DrawLine(window, p1.x + window->w/2, -p1.y + window->h / 2, p2.x + window->w / 2, -p2.y + window->h / 2, 255, 255, 255);
		if(i != o.nbEdges) e = e->next;
	}
}