#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "utils_SDL.h"
#include "moteur3D.h"

int inArray(int id, int tab[50]) {

	for (int i = 0; i < 50; i++) {
		if (id == tab[i]) return 1;
	}
	return 0;

}

///////////////////////PARTIE OBJETS///////////////////////

cam initCam(float x, float y, float z, float lon, float lat, float zoom) {

	cam c;
	c.x = x;
	c.y = y;
	c.z = z;
	c.lon = lon;
	c.lat = lat;
	c.zoom = zoom;
	return c;

}

void addCam(cam* c, float x, float y, float z, float lon, float lat, float zoom) {

	c->x += x;
	c->y += y;
	c->z += z;
	c->lon += lon;
	//while (c->lon >= 2 * PI) c->lon -= 2 * PI;
	//while (c->lon < 0) c->lon += 2 * PI;
	c->lat += lat;
	//if (c->lat > PI/2) c->lat = PI/2;
	//if (c->lat < -PI/2) c->lat = -PI/2;
	c->zoom += zoom;
	if (c->zoom > 10) c->zoom = 10.0;

}

point* createPoint(float x, float y, float z) {

	point* p = (point*)calloc(1, sizeof(point));
	p->x = x;
	p->y = y;
	p->z = z;
	return p;

}

obj* createObject(point* p) {

	p->id = 0;
	obj* o = (obj*)calloc(1, sizeof(obj));
	o->head = p;
	o->size = 1;
	return o;

}

void addObject(obj** head, obj* o) {

	if (!head) return;
	if (*head) o->next = *head;
	*head = o;

}

point* pointById(obj* o, int id) {

	if (!o) return NULL;
	point* p = o->head;

	while (p) {
		if (p->id == id) return p;
		p = p->next;
	}
	return NULL;

}

void addNeighbor(obj* o, int id1, int id2) {

	if (!o) return;
	point* p1 = pointById(o, id1);
	point* p2 = pointById(o, id2);
	printf("%d et %d trouves\n", p1->id, p2->id);

	if (!p1 || !p2) return;
	vois* v = (vois*)calloc(1, sizeof(vois));
	vois* vr = p2->voisins;
	point* p = p1;
	if (id1 < id2) {
		vr = p1->voisins;
		p = p2;
	}

	v->p = p;
	v->vnext = vr;
	if (id1 < id2) {
		p1->voisins = v;
	}
	else {
		p2->voisins = v;
	}

}

void addPoint(obj** o, point* p) {

	if (!o) return;
	if (!*o) {
		*o = createObject(p);
		return;
	}
	p->id = (*o)->size;
	p->next = (*o)->head;
	(*o)->head = p;
	(*o)->size++;

}

void remPoint(obj* o, int id) {

	point* p = o->head;

	if (p && p->id == id) {
		o->head = p->next;
		freePoint(p);
		return;
	}

	while (p->next && p->next->id != id) p = p->next;
	if (!p->next) return;

	point* tmp = p->next;
	if (p->next->next) p->next = p->next->next;
	freePoint(tmp);

}

void freePoint(point* p) {

	free(p->voisins);
	free(p);

}

void affObject(obj* o) {

	printf("*");
	if (!o) return;

	point* p = o->head;

	while (p) {
		printf("[%d:(%.1f,%.1f,%.1f)]->", p->id, p->x, p->y, p->z);
		p = p->next;
	}

	printf("\n");

}

///////////////////////PARTIE GEOMETRIE///////////////////////

screen getScreen(cam c) {

	screen s;

	// De coordonnées sphériques à cartésiennes.

	float cola = cos(c.lat);
	float sila = sin(c.lat);
	float colo = cos(c.lon);
	float silo = sin(c.lon);
	s.vect_x = 50 * cola * colo;
	s.vect_y = 50 * cola * silo;
	s.vect_z = 50 * sila;

	s.unit1_x = -silo;
	s.unit1_y = colo;
	s.unit1_z = 0;

	s.unit2_x = -sila * colo;
	s.unit2_y = -sila * silo;
	s.unit2_z = cola;

	s.pos_x = c.x + s.vect_x;
	s.pos_y = c.y + s.vect_y;
	s.pos_z = c.z + s.vect_z;
	
	return s;

}

pointS pointOnScreen(cam c, screen s, point* p) {

	pointS v;
	float k = (s.vect_x*(c.x - s.pos_x) + s.vect_y*(c.y - s.pos_y) + s.vect_z*(c.z - s.pos_z))/(s.vect_x*(c.x - p->x) + s.vect_y*(c.y - p->y) + s.vect_z*(c.z - p->z));
	v.display = 1;
	if (k > 1 || k < 0) v.display = 0;
	float x, y, z;
	
	//printf("POINT CAM : x=%.1f y=%.1f z=%.1f\n", c.x, c.y, c.z);
	//printf("POINT PLAN : x=%.1f y=%.1f z=%.1f\n", s.pos_x, s.pos_y, s.pos_z);
	//printf("VECT PLAN : x=%.1f y=%.1f z=%.1f\n", s.vect_x, s.vect_y, s.vect_z);
	//printf("UNIT1: x=%f y=%f z=%f\n", s.unit1_x, s.unit1_y, s.unit1_z);
	//printf("UNIT2 : x=%f y=%f z=%f\n", s.unit2_x, s.unit2_y, s.unit2_z);
	//printf("POINT OBS : x=%.1f y=%.1f z=%.1f\n", p->x, p->y, p->z);
	x = -((c.x - p->x) * k + c.x);
	y = -((c.y - p->y) * k + c.y);
	z = -((c.z - p->z) * k + c.z);

	//printf("x=%.2f y=%.2f z=%.2f, k=%f, display=%d\n",x,y,z,k,v.display);
	
	float d = s.unit1_x * s.unit2_z - s.unit1_z * s.unit2_x;
	if ((int)(d * 100) != 0) {

		//printf("X = %f ; Y = %f\n", (s.unit2_z * x - s.unit2_x * z) / d, (s.unit1_x * z - s.unit1_z * x) / d);
		v.m_x = round((s.unit2_z * x - s.unit2_x * z) / d);
		v.m_y = round((s.unit1_x * z - s.unit1_z * x) / d);
		//printf("1\n");

	}
	if ((int)((d = s.unit1_x * s.unit2_y - s.unit1_y * s.unit2_x) * 100) != 0) {

		//printf("X = %f ; Y = %f\n", (s.unit2_x * y - s.unit2_y * x) / d, (s.unit1_y * x - s.unit1_x * y) / d);
		v.m_x = round((s.unit2_x * y - s.unit2_y * x) / d);
		v.m_y = round((s.unit1_y * x - s.unit1_x * y) / d);
		//printf("2\n");

	}
	if ((int)((d = s.unit1_y * s.unit2_z - s.unit1_z * s.unit2_y) * 100) != 0) {
		
		//printf("X = %f ; Y = %f, id:%d\n", (s.unit2_z * y - s.unit2_y * z) / d, (s.unit1_y * z - s.unit1_z * y) / d, p->id);
		v.m_x = round((s.unit2_z * y - s.unit2_y * z) / d);
		v.m_y = round((s.unit1_y * z - s.unit1_z * y) / d);
		//printf("3\n");

	}
	/*if (v.m_x < -320 || v.m_x > 320) { ;
		v.m_x = (int)(v.m_x * 320 / (float)abs(v.m_x));
		v.m_y = (int)(v.m_y * 320 / (float)abs(v.m_x));
	}
	if (v.m_y < -240 || v.m_y > 240) {
		v.m_x = (int)(v.m_x * 240 / (float)abs(v.m_y));
		v.m_y = (int)(v.m_y * 240 / (float)abs(v.m_y));
	}*/
	v.m_x *= c.zoom;
	v.m_y *= c.zoom;
	v.m_x += 320*(1 - sin(c.lon));
	v.m_y += 240*(1 - sin(c.lat));
	
	return v;

}

void displayAll(SDL_Surface* window, cam c, obj* head) {//TO FINISH

	int tab[50];
	int i;
	Uint32 col = SDL_MapRGB(window->format, 255, 255, 255);
	screen s = getScreen(c);
	pointS v1;
	pointS v2;
	point* p;
	vois* voisins;

	//affObject(head);

	while (head) {

		i = 0;
		memset(tab, 0, 50);
		p = head->head;

		while (p) {

			tab[i++] = p->id;
			voisins = p->voisins;
			v1 = pointOnScreen(c, s, p);

			while (voisins) {

				tab[i++] = voisins->p->id;
				//printf("LIEN ENTRE [%.1f;%.1f;%.1f] ET [%.1f;%.1f;%.1f]\n", p->x,p->y,p->z,voisins->p->x, voisins->p->y, voisins->p->z);

				v2 = pointOnScreen(c, s, voisins->p);
				if (v2.display == 1 && v1.display == 1)_SDL_DrawLine(window, v1.m_x, v1.m_y, v2.m_x, v2.m_y, 255, 255, 255);
				voisins = voisins->vnext;
				//printf("[%d;%d] %d\n", v2.m_x, v2.m_y, v2.display);

			}
			if (v1.display == 0) _SDL_SetPixel(window, v1.m_x, v1.m_y, col);
			//printf("[%d;%d] %d\n", v1.m_x, v1.m_y, v1.display);

			p = p->next;
		}

		head = head->next;

	}

}