#include "mot.h"
#include "utils_SDL.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdarg.h>

void printP3(point3 p) {
	printf("[%.1f , %.1f , %.1f]", p.x, p.y, p.z);
}

int isInPlane(face f, point3* p) {

	//Vecteur orthonormal à la normale ?
	point3 v = sum(*f.points->p, *p, -1);
	point3 n = f.normale;
	return !(n.x * v.x + n.y * v.y + n.z * v.z);

}

int checkFaceDisplay(face f, cam c) {

	point3 vcf = sum(f.G, c.pos, -1);
	point3 nf = f.normale;

	if (c.pos.x > 1500) {
		printf("");
	}

	if (scalar(vcf, nf) < 0 && scalar(c.dir, vcf) > 0) {
		listP* p = f.points;
		for (int i = 0; i < f.nbPoints; i++) {
			if (p->p->p.display) return 1;
			p = p->next;
		}
	}
	return 0;
}

listP* createListP(point3* p) {
	listP* lp = (listP*)calloc(1, sizeof(listP));
	lp->p = p;
	return lp;
}

void printFace(face f) {
	printP3(f.normale);
	printf(" : ");
	listP* p = f.points;
	for (int i = 0; p; i++) {

		printP3(*p->p);
		printf(" -> ");
		p = p->next;
	}
	printf("\n");
}

///////////////////////////////////////////////////////////

// 2D
point sum2(point p1, point p2, int diff) {
	
	if (diff != -1) diff = 1;

	p1.x = round(p1.x + p2.x * diff);
	p1.y = round(p1.y + p2.y * diff);

	return p1;
}

float norm2(point p) {
	return sqrtf(p.x*p.x + p.y*p.y);
}

point scale2(point p, float n) {
	p.x *= n;
	p.y *= n;
	return p;
}

point getUnitV2(point p) {
	return scale2(p, 1.f / norm2(p));
}

// 3D
point3 sum(point3 p1, point3 p2, int diff) {

	if (diff != -1) diff = 1;

	p1.x += p2.x * diff;
	p1.y += p2.y * diff;
	p1.z += p2.z * diff;
	
	return p1;
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

point3 getUnitV(point3 v) {
	return scale(v, 1 / norm(v));
}

// A revoir
float scalar(point3 p1, point3 p2) {
	return p1.x * p2.x + p1.y * p2.y + p1.z * p2.z;
}

float norm(point3 p) {
	return sqrtf(p.x * p.x + p.y * p.y + p.z * p.z);
}

///////////////////////////////////////////////////////////

// Points methods

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

// Faces methods

face* createFace(Uint32 color, int nbPoints, ...) {

	if (nbPoints < 3) return NULL;

	va_list list;
	va_start(list, nbPoints);

	point3 p1 = *va_arg(list, point3*);
	point3 p2 = *va_arg(list, point3*);
	point3 p3 = *va_arg(list, point3*);

	point3 v12 = sum(p2, p1, -1);
	point3 v13 = sum(p3, p1, -1);

	point3 normale = setPoint(
		(v12.z * v13.y - v12.y * v13.z),
		(v12.x * v13.z - v12.z * v13.x),
		(v12.y * v13.x - v12.x * v13.y)
	);

	face* f = (face*)calloc(1,sizeof(face));
	f->normale = getUnitV(normale);

	va_start(list, nbPoints);
	listP* p = createListP(va_arg(list, point3*));
	listP* fp = p;

	float x = 0;
	float y = 0;
	float z = 0;

	for (int i = 1; i < nbPoints; i++) {
		x += p->p->x;
		y += p->p->y;
		z += p->p->z;
		p->next = createListP(va_arg(list, point3*));
		p = p->next;
	}

	x += p->p->x;
	y += p->p->y;
	z += p->p->z;

	va_end(list);

	f->points = fp;
	f->G = setPoint(x / nbPoints, y / nbPoints, z / nbPoints);
	f->color = color;
	f->nbPoints = nbPoints;
	f->next = NULL;

	return f;

}

void addPointFace(face *f, point3* p) {

	if (!isInPlane(*f, p)) return f;

	f->nbPoints++;
	//Push
	listP* lp = (listP*)calloc(1, sizeof(listP));
	lp->p = p;
	lp->next = f->points;
	f->points = lp;

	return f;

}

// Spheres methods

void addSphere(listS** S, sphere* s) {
	if (!S) return;
	if (!*S) {
		*S = createListS(s);
		return;
	}

	s->pos->next = (*S)->s->pos;
	listS* tmp = createListS(s);
	tmp->next = *S;
	*S = tmp;
}

listS* createListS(sphere* s) {
	listS* S = (listS*)calloc(1, sizeof(listS));
	S->next = NULL;
	S->s = s;
	return S;
}

sphere* createSphere(float radius, point3* pos, Uint32 color) {
	sphere* s = (sphere*)calloc(1, sizeof(sphere));
	s->radius = radius;
	s->color = color;
	pos->next = NULL;
	s->pos = pos;
	return s;
}

// Object
obj* createObj3D(face* f, point3* p, int nbFaces, int nbVertexes, int isStatic) {

	obj* o = (obj*)calloc(1, sizeof(obj));

	o->nbFaces = nbFaces;
	o->nbVertexes = nbVertexes;
	o->isStatic = isStatic;

	o->faces = f;
	o->vertexes = p;

	float x = 0;
	float y = 0;
	float z = 0;
	for (int i = 0; i < nbVertexes; i++) {
		x += p->x;
		y += p->y;
		z += p->z;
		p = p->next;
	}
	o->G = setPoint(x / nbVertexes, y / nbVertexes, z / nbVertexes);

	// Face normals orientation correction
	point3 vfo;
	point3 nf;
	for (int i = 0; i < nbFaces; i++) {

		vfo = sum(o->G, f->G, -1);
		nf = f->normale;
		if (scalar(vfo, nf) > 0) f->normale = scale(nf, -1);

		f = f->next;
	}

	return o;

}

obj* createCube(point3 pos, point3 rot, float size, Uint32 color, int isStatic) {

	size /= 2;

	float cosX = cos(rot.x);
	float sinX = sin(rot.x);
	float cosY = cos(rot.y);
	float sinY = sin(rot.y);
	float cosZ = cos(rot.z);
	float sinZ = sin(rot.z);

	point3* a = createPoint(pos.x - size, pos.y + size, pos.z - size);
	point3* b = createPoint(pos.x - size, pos.y + size, pos.z + size);
	point3* c = createPoint(pos.x - size, pos.y - size, pos.z + size);
	point3* d = createPoint(pos.x - size, pos.y - size, pos.z - size);
	point3* e = createPoint(pos.x + size, pos.y + size, pos.z - size);
	point3* f = createPoint(pos.x + size, pos.y + size, pos.z + size);
	point3* g = createPoint(pos.x + size, pos.y - size, pos.z + size);
	point3* h = createPoint(pos.x + size, pos.y - size, pos.z - size);

	point3* P[8] = { a, b, c, d, e, f, g, h };
	for (int i = 7; i > 0; i--) P[i - 1]->next = P[i];

	face* F[6] = {
		createFace(color, 4, a, b, c, d),
		createFace(color, 4, e, f, g, h),
		createFace(color, 4, a, b, f, e),
		createFace(color, 4, c, d, h, g),
		createFace(color, 4, a, d, h, e),
		createFace(color, 4, b, c, g, f),
	};
	for (int i = 5; i > 0; i--) F[i - 1]->next = F[i];

	obj* o = createObj3D(*F, *P, 6, 8, isStatic);
	RotateObj(o, rot);

	return o;
}

void addObj(listO** O, obj* o) {

	listO* newO = (listO*)calloc(1, sizeof(listO));
	newO->o = o;
	newO->next = *O;
	*O = newO;

}

///////////////////////////////////////////////////////////

// Camera
cam initCam(point3 pos, point3 dir) {

	cam c;
	c.pos = pos;
	c.dir = dir;
	c.dist = 150;
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
	if (C->lon > PI) C->lon = C->lon - 2 * PI;
	else if (C->lon < -PI) C->lon = C->lon + 2 * PI;
	C->lat += lat;
	if (C->lat > 0.99 * PI / 2) C->lat = 0.99 * PI / 2;
	else if (C->lat < -0.99 * PI / 2) C->lat = -0.99 * PI / 2;

	lon = C->lon;
	lat = C->lat;

	float normDir = norm(C->dir);
	C->dir = setPoint(normDir * cos(lat) * cos(lon), normDir * sin(lat), normDir * cos(lat) * sin(lon));

	point3 dir = C->dir;
	float nNorm = sqrtf(dir.x * dir.x + dir.y * dir.y) / normDir;

	C->vUnit1 = setPoint(-normDir * sin(lon), 0, normDir * cos(lon));
	C->vUnit2 = setPoint(-normDir * sin(lat) * cos(lon), normDir * cos(lat), -normDir * sin(lat) * sin(lon));

	C->pCam = sum(C->pos, scale(C->dir, C->dist), 0);
}

// Light
void addLight(listL** L, light* l) {
	if (!L) return;
	if (!*L) {
		*L = createListL(l);
		return;
	}
	l->pos->next = (*L)->l->pos;
	listL* tmp = createListL(l);
	tmp->next = *L;
	*L = tmp;
}

listL* createListL(light* l) {
	listL* L = (listL*)calloc(1, sizeof(listL));
	L->next = NULL;
	L->l = l;
	return L;
}

light* createLight(point3* pos, float intensity, Uint32 color) {

	light* l = (light*)calloc(1, sizeof(light));
	l->pos = pos;
	l->intensity = intensity;
	l->color = color;

	return l;

}

void applyLights(face f, cam c, listL* L, Uint8 color[3]) {

	point3 normal = f.normale;
	point3 vfc = getUnitV(sum(c.pos, f.G, -1)), vlf, vr;
	light* l;
	Uint8* cf = (Uint8*)&f.color, *cl;
	Uint8 colorSum[3] = { 0, 0, 0 };
	float nlf, coef, coefSum = 1.f;

	while (L) {
		l = L->l;
		vlf = sum(f.G, *l->pos, -1);

		// Getting direction of the reflected light vector on the face
		vr = getUnitV(sum(getUnitV(f.normale), getUnitV(vlf), 0));

		nlf = norm(vlf) / 10;

		// Scalar product with the vector face-camera with all the other bullshit tweaked on ass makes the brightness coef
		coef = (scalar(vr, vfc) * 4 - scalar(normal, vlf) / 4) * l->intensity / (nlf * nlf);

		Uint8* cl = (Uint8*)&l->color;

		coefSum += coef;
		colorSum[0] += (coef < 0) ? 0 : cl[0] * coef;
		colorSum[1] += (coef < 0) ? 0 : cl[1] * coef;
		colorSum[2] += (coef < 0) ? 0 : cl[2] * coef;

		L = L->next;
	}

	// Moyenne des couleurs ajoutées
	color[2] = (cf[0] + colorSum[2]) / coefSum;
	color[1] = (cf[1] + colorSum[1]) / coefSum;
	color[0] = (cf[2] + colorSum[0]) / coefSum;


}

// Render
void pointsTo2dProjection(int screenW, int screenH, point3* p, cam c) {
	float normDir = norm(c.dir);

	// ref point on the cam plane
	point3 pcam = c.pCam;
	point3 pos = c.pos;
	point3 dir = c.dir;

	// i and j unit vectors on the cam plane
	point3 vUnit1 = c.vUnit1;
	point3 vUnit2 = c.vUnit2;

	// vector point to camera
	point3 vpc;

	// (X - pcamX)*dirX + (Y - pcamY)*dirY + (Z - pcamZ)*dirZ = 0
	// Cartesian d value in [ ax + by + cz = -d ] for the cam plane
	float d = (pcam.x * dir.x) + (pcam.y * dir.y) + (pcam.z * dir.z);
	float n,k, kbis, nbis, den;
	point pProj;

	// Intersection coordinates
	float X, Y, Z;

	while(p) {

		// v is vector from point to cam (p to c) we want k such that v*k + p is a point of cam plane
		// so that [ ax + by + cz = -d ] <=> [ a*(vx*k + px) + b*(vy*k + py) + c*(vz*k + pz) = -d ] <=> [ k = -(d + a*px + b*py + c*pz)/(a*vx + b*vy + c*vz) ] with denom != 0
		// if k > 1, point is behind cam plane so must not be visible
		// if k < 0, point is between cam and plane.
	
		vpc = sum(pos, *p, -1);
		// n = dir.x * (pos.x - p->x) + dir.y * (pos.y - p->y) + dir.z * (pos.z - p->z);
		n = scalar(dir, vpc);

		if (n == 0) {
			p->p.display = 0;
			p = p->next;
			continue;
		}

		k = (d - dir.x * p->x - dir.y * p->y - dir.z * p->z) / n;
		if (k > 1) {
			p->p.display = 0;

			X = (pos.x - p->x) * k + p->x - pcam.x;
			Y = (pos.y - p->y) * k + p->y - pcam.y;
			Z = (pos.z - p->z) * k + p->z - pcam.z;

			if (fabs(den = vUnit1.x * vUnit2.y - vUnit1.y * vUnit2.x) > 0.00001) {
				pProj.x = (vUnit2.y * X - vUnit2.x * Y) * normDir / den + screenW;
				pProj.y = (vUnit1.y * X - vUnit1.x * Y) * normDir / den + screenH;
			}
			else if (fabs(den = vUnit1.x * vUnit2.z - vUnit1.z * vUnit2.x) > 0.00001) {
				pProj.x = (vUnit2.z * X - vUnit2.x * Z) * normDir / den + screenW;
				pProj.y = (vUnit1.z * X - vUnit1.x * Z) * normDir / den + screenH;
			}
			else {
				den = vUnit1.y * vUnit2.z - vUnit1.z * vUnit2.y;
				pProj.x = (vUnit2.z * Y - vUnit2.y * Z) * normDir / den + screenW;
				pProj.y = (vUnit1.z * Y - vUnit1.y * Z) * normDir / den + screenH;
			}

			point p2loin;
			X = pos.x - pcam.x;
			Y = pos.y - pcam.y;
			Z = pos.z - pcam.z;
			if (fabs(den = vUnit1.x * vUnit2.y - vUnit1.y * vUnit2.x) > 0.00001) {
				p2loin.x = (vUnit2.y * X - vUnit2.x * Y) * normDir / den + screenW;
				p2loin.y = (vUnit1.y * X - vUnit1.x * Y) * normDir / den + screenH;
			}
			else if (fabs(den = vUnit1.x * vUnit2.z - vUnit1.z * vUnit2.x) > 0.00001) {
				p2loin.x = (vUnit2.z * X - vUnit2.x * Z) * normDir / den + screenW;
				p2loin.y = (vUnit1.z * X - vUnit1.x * Z) * normDir / den + screenH;
			}
			else {
				den = vUnit1.y * vUnit2.z - vUnit1.z * vUnit2.y;
				p2loin.x = (vUnit2.z * Y - vUnit2.y * Z) * normDir / den + screenW;
				p2loin.y = (vUnit1.z * Y - vUnit1.y * Z) * normDir / den + screenH;
			}


			pProj.x += 2 * (p2loin.x - pProj.x) ;
			pProj.y += 2 * (p2loin.y - pProj.y);

			float coef = (p2loin.y - pProj.y) / (p2loin.x - pProj.x);
			if (fabs(coef) < (float)p2loin.y / p2loin.x) {
				if (pProj.x < screenW) {
					p->p.x = 0.f;
					p->p.y = pProj.y - pProj.x * coef;
				}
				else {
					p->p.x = 2 * screenW - 1;
					p->p.y = pProj.y - (pProj.x - 2 * screenW) * coef;
				}
			}
			else {
				if (pProj.y < screenH) {
					p->p.x = pProj.x - pProj.y / coef;
					p->p.y = 0.f;
				}
				else {
					p->p.x = pProj.x - (pProj.y - 2 * screenH) / coef;
					p->p.y = 2 * screenH - 1;
				}
			}

			p = p->next;
			continue;
		}
		
		p->p.display = 1;

		// P = v*k + p
		X = (pos.x - p->x) * k + p->x + pcam.x;
		Y = (pos.y - p->y) * k + p->y + pcam.y;
		Z = (pos.z - p->z) * k + p->z + pcam.z;		

		/*
		* x*i + y*j = (X ; Y ; Z) => x*v1.x + y*v2.x = X | x*v1.y + y*v2.y = Y | x*v1.z + y*v2.z = Z
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
		*
		* Taking 2 equations of the system, we got the following matrices operation :
		* [x][v1.x v2.x] = [X] <=> [x] = [X][v1.x v2.x]^-1
		* [y][v1.y v2.y] = [Y]	   [y] = [Y][v1.y v2.y]
		* Verifying that v1.x*v2.y - v1.y*v2.x != 0
		* if not select another combinaison of 2 equations in the system
		*/
		if (fabs(den = vUnit1.x * vUnit2.y - vUnit1.y * vUnit2.x) > 0.00001) {
			p->p.x = (vUnit2.y * X - vUnit2.x * Y) * normDir / den + screenW;
			p->p.y = (vUnit1.y * X - vUnit1.x * Y) * normDir / den + screenH;
		}
		else if (fabs(den = vUnit1.x * vUnit2.z - vUnit1.z * vUnit2.x) > 0.00001) {
			p->p.x = (vUnit2.z * X - vUnit2.x * Z) * normDir / den + screenW;
			p->p.y = (vUnit1.z * X - vUnit1.x * Z) * normDir / den + screenH;
		}
		else {
			den = vUnit1.y * vUnit2.z - vUnit1.z * vUnit2.y;
			p->p.x = (vUnit2.z * Y - vUnit2.y * Z) * normDir / den + screenW;
			p->p.y = (vUnit1.z * Y - vUnit1.y * Z) * normDir / den + screenH;
		}

		if (p->p.display == 0) {
			//p->p.x += 2 * (screenW - p->p.x);
			//p->p.y += 2 * screenH;
		}
			 
		p->p.depth = norm(vpc);
		p = p->next;
	}

}

void drawSpheres(SDL_Surface* window, int* Z_Buffers, listS* S, listL* L, cam c, int alt) {

	int w = window->w;
	int h = window->h;
	Uint32* pxls = window->pixels;

	pointsTo2dProjection(w / 2, h / 2, S->s->pos, c);

	int x, y, slot, rad2, lx, R, G, B, *Z_buffer, depth;
	float coef = 150.f, dist, coefl, normsl, lum, lightDir;
	sphere* s;
	point p, pl;
	point3 psc, psl, pl3;
	light* l = L->l; // TODO : faire avec toutes les lumières
	Uint8* colorS, color[3], *cl = &l->color;
	while (S) {
		s = S->s;
		p = s->pos->p;

		if (p.display == 0) {
			S = S->next;
			continue;
		}

		psc = sum(c.pos, *s->pos, -1);
		psl = sum(*l->pos, *s->pos, -1);

		// Light handling : pl is the nearest point of the sphere from the light
		lightDir = scalar(psc, psl); // A faire plus tard : quand la sphere est entre la lumiere et nous, prendre le point opposé au point pl et ajouter coefl quand norm augmente
		normsl = norm(psl);
		pl3 = sum(*s->pos, scale(psl, s->radius / normsl), 0);
		pointsTo2dProjection(w / 2, h / 2, &pl3, c);
		pl = sum2(pl3.p, p, -1);

		//pl = scale2(sum2(l->pos->p, p, -1), s->radius / normsl); // Amélioration : prendre le vrai point pl
		colorS = (Uint8*)&s->color;

		dist = fabs(scalar(psc, c.dir));
		rad2 = coef * s->radius / dist;
		lum = 100000.f * l->intensity * rad2 * rad2 * rad2 * rad2 / (normsl * normsl * normsl);
		for (y = -rad2; y <= rad2; y++) {

			if (p.y + y < 0 || p.y + y >= h) continue;

			lx = sqrt(fabs(rad2 * rad2 - y * y));

			for (x = -lx; x <= lx; x++) {

				if (p.x + x < 0 || p.x + x >= w) continue;

				slot = (int)(p.x + x) + (int)(p.y + y) * window->w;

				Z_buffer = Z_Buffers + slot;

				depth = (p.depth - s->radius + sqrt(x * x + y * y)) * alt; //(courbe de la boule pour + de précision)
				if (*Z_buffer && !((alt == 1) ? (*Z_buffer < 0 || depth < *Z_buffer) : (*Z_buffer > 0 || depth > *Z_buffer))) continue;
				*Z_buffer = depth;

				coefl = lum / (pow((pl.x - x) * (pl.x - x) + (pl.y - y) * (pl.y - y), 1.3f) + 0.00001);


				if (coefl > 0.01) {
					color[0] = (R = ((colorS[0] + cl[0] * coefl) / (1 + coefl))) > 255 ? 255 : R;
					color[1] = (G = ((colorS[1] + cl[1] * coefl) / (1 + coefl))) > 255 ? 255 : G;
					color[2] = (B = ((colorS[2] + cl[2] * coefl) / (1 + coefl))) > 255 ? 255 : B;
					*(pxls + slot) = *(Uint32*)color;
				}
				else *(pxls + slot) = *(Uint32*)colorS;

			}

		}
		//if (!(pl.x + p.x < 0 || pl.x + p.x >= w || pl.y + p.y < 0 || pl.y + p.y >= h)) *(pxls + (int)(pl.x + p.x) + (int)(pl.y + p.y) * w) = (255 << 16);
		S = S->next;
	}
	
}

void drawLine(SDL_Surface* window, point p1, point p2, Uint32 color) {

	int w = window->w;
	int h = window->h;
	Uint32* pxls = window->pixels;

	point v = sum2(p2, p1, -1), p;
	point v_unit = getUnitV2(v);
	float d = 0.f, n = norm2(v);

	if (p1.x < 0) d = -(float)p1.x / v_unit.x;
	else if (p1.x >= w) d = (float)(w - p1.x) / v_unit.x;
	if (p1.y + d * v_unit.y < 0) d = -(float)p1.y / v_unit.y;
	else if (p1.y + d * v_unit.y >= h) d = (float)(h - p1.y) / v_unit.y;

	for (; d < n; d+=1.5f) {
		p = sum2(p1, scale2(v_unit, d), 0);
		if (p.x < 0 || p.x >= w || p.y < 0 || p.y >= h) return;
		*(pxls + (int)(p.x + p.y * w)) = color;
	}

}

void displayLights(SDL_Surface* window, int* Z_buffers, listL* L, cam c, int alt) {

	int w = window->w;
	int h = window->h;

	pointsTo2dProjection(w / 2, h / 2, L->l->pos, c);

	float coef = 700.f, dist, norm;
	int rad2, lx, R, G, B, *Z_buffer, depth;

	Uint32* pxls = window->pixels;
	Uint8* pxl;

	light* l;
	Uint8* color;
	point p;
	while (L) {
		l = L->l;
		p = l->pos->p;

		Z_buffer = Z_buffers + (int)p.x + (int)(p.y) * w;
		depth = p.depth * alt;

		if (
			p.display == 0 || (p.x < 0 || p.x > w || p.y < 0 || p.y > h) ||
			(*Z_buffer && !((alt == 1) ? (*Z_buffer < 0 || depth < *Z_buffer) : (*Z_buffer > 0 || depth > *Z_buffer)))
			) {
			L = L->next;
			continue;
		}

		color = (Uint8*)&l->color;

		dist = fabs(scalar(sum(*l->pos, c.pos, -1), c.dir));

		rad2 = coef * l->intensity / dist;
		for (int y = -rad2; y <= rad2; y++) {

			if (p.y + y < 0.f || p.y + y >= h) continue;

			lx = sqrt(fabs(rad2 * rad2 - y * y));
			for (int x = -lx; x <= lx; x++) {

				if (p.x + x < 0.f || p.x + x >= w) continue;

				norm = pow((x * x + y * y), 1.2) / rad2 + 0.001;
				//if (norm > 20) continue;

				pxl = (Uint8*)(pxls + (int)p.x + x + ((int)p.y + y) * w);
				R = pxl[0] + (float)color[0] / norm;
				G = pxl[1] + (float)color[1] / norm;
				B = pxl[2] + (float)color[2] / norm;

				pxl[0] = R > 255 ? 255 : R;
				pxl[1] = G > 255 ? 255 : G;
				pxl[2] = B > 255 ? 255 : B;
			}
			
			//colorRow(window, Z_buffers, p.x - x, p.x + x, p.y + y, p.depth, p.depth, l->color, alt);
		}

		L = L->next;
	}

}

void displayFloorLines(SDL_Surface* window, int* Z_Buffers, cam c) {

	point3 cPos = c.pos;
	
	float radius = 1050;
	float gridSize = 50;
	int ratio = (int)(radius / gridSize);
	int nbPoints = 8 * ratio;

	float x, z, x_start, z_start;
	x_start = round(cPos.x / gridSize) * gridSize;
	z_start = round(cPos.z / gridSize) * gridSize;

	point3** P = (point3**)calloc(nbPoints, sizeof(point3*));

	for (int i = -ratio; i < ratio; i++) {
		// (X - Cx)² + (Y - Cy)² = radius² => X = sqrtf(radius² - (Y - Cy)²) + Cx
		z = z_start + gridSize * i;
		x = sqrtf(radius * radius - pow(z - cPos.z, 2));
		P[4 * (i + ratio)] = createPoint(cPos.x + x, 0.f ,z);
		P[4 * (i + ratio) + 1] = createPoint(cPos.x - x, 0.f, z);

		x = x_start + gridSize * i;
		z = sqrtf(radius * radius - pow(x - cPos.x, 2));
		P[4 * (i + ratio) + 2] = createPoint(x, 0.f, cPos.z + z);
		P[4 * (i + ratio) + 3] = createPoint(x, 0.f, cPos.z - z);

	}

	for (int i = nbPoints - 1; i > 0; i--) P[i - 1]->next = P[i];

	pointsTo2dProjection(window->w/2, window->h/2, P[0], c);

	for (int i = 0; i < nbPoints; i += 2) {
		drawLine(window, P[i]->p, P[i + 1]->p, SDL_MapRGB(window->format ,255 * (i == (float)nbPoints / 2), 150, 0));
	}

	free(P);
}

void displayObj(SDL_Surface* window, int* Z_Buffers, obj o, cam c, listL* L, int camHasMoved, int alt) {

	if (camHasMoved) pointsTo2dProjection(window->w/2, window->h/2, o.vertexes, c);

	face* f = o.faces;
	Uint8 color[3];

	for (int i = 0; i < o.nbFaces; i++) {

		if (camHasMoved) f->display = checkFaceDisplay(*f, c);

		if (f->display) {
			applyLights(*f, c, L, color);
			colorFace(window, Z_Buffers, f->points, f->nbPoints, SDL_MapRGB(window->format, color[0], color[1], color[2]), alt);
		}
		
		f = f->next;
	}
	
}

void colorRow(SDL_Surface* window, int* Z_Buffers, int x1, int x2, int y,  int z1, int z2, const Uint32 color, int alt) {

	int w = window->w;
	int h = window->h;
	if (y < 0 || y >= h || x1 == x2 || (x1 < 0 || x1 >= w) && (x2 < 0 || x2 >= w)) return;

	int inc = (x1 < x2) ? 1 : -1;
	int x = (x1 < x2) ? x1 : x2;
	int l = abs(x1 - x2);

	int z = (z1 < z2) ? z1 : z2;
	int dz = abs(z1 - z2);

	int currentZ;
	int slot;
	int* Z_buffer;

	for (int i = 0; i <= l; i += 1) {

		// Uncomment to turn off the Occlusion culling
		//if ((x + i >= 0 && x + i < window->clip_rect.w) && (y >= 0 && y < window->clip_rect.h)) *((Uint32*)(window->pixels) + x + i + y * w) = color;
		//continue;

		if (x + i < 0 || x + i >= w) continue;

		currentZ = (z + dz * i / l) * alt;
		slot = x + i + y * w;
		Z_buffer = (Z_Buffers + slot);

		// Occlusion culling
		if (!*Z_buffer || ((alt == 1) ? (*Z_buffer < 0 || currentZ < *Z_buffer) : (*Z_buffer > 0 || currentZ > *Z_buffer))) {
			*Z_buffer = currentZ;
			*((Uint32*)(window->pixels) + slot) = color;
		}
	}

}

void colorTriangle(SDL_Surface* window, int* Z_Buffers, point p, point p2, point p3, const Uint32 color, int alt) {

	int max_y = (p.y > p2.y) ? (p.y > p3.y ? p.y : p3.y) : (p2.y > p3.y ? p2.y : p3.y);
	int min_y = (p.y < p2.y) ? (p.y < p3.y ? p.y : p3.y) : (p2.y < p3.y ? p2.y : p3.y);

	point v12 = sum2(p2, p, -1);
	point v13 = sum2(p3, p, -1);
	point v23 = sum2(p3, p2, -1);

	int z1 = p.depth;
	int z2 = p2.depth;
	int z3 = p3.depth;

	int z12 = z2 - z1;
	int z13 = z3 - z1;
	int z23 = z3 - z2;

	/*
	*         *                      Filling the triangle from top to bottom
	*        *===*					 by finding the X of 2 of the edges in its
	*       *=======*                intersection with the Y value. 
	*      *===========*             v.x/v.y being the slope value from the
	*---->*===============*<-------- Y axis perspective.
	*    *                   *      
	*   *                       *   
	*  *'''''''''''''''''''''''''''*
	*/

	for (int y = min_y + (v12.y != 0 && v13.y != 0 && v23.y != 0); y < max_y; y+=1) {
		if (y < 0 || y >= window->h) continue;
		if      ((p.y < y && y <= p2.y || p.y > y && y >= p2.y) && (p.y < y && y <= p3.y || p.y > y && y >= p3.y) && (v12.y != 0 && v13.y != 0))	colorRow(window, Z_Buffers, p.x + (y - p.y) * v12.x / v12.y, p.x + (y - p.y) * v13.x / v13.y,	y, z1 + z12 * (y - p.y) / v12.y, z1 + z13 * (y - p.y) / v13.y,	color, alt);
		else if ((p.y <= y && y < p2.y || p.y >= y && y > p2.y) && (p2.y < y && y <= p3.y || p2.y > y && y >= p3.y) && (v12.y != 0 && v23.y != 0))	colorRow(window, Z_Buffers, p.x + (y - p.y) * v12.x / v12.y, p2.x + (y - p2.y) * v23.x / v23.y, y, z1 + z12 * (y - p.y) / v23.y, z2 + z23 * (y - p2.y) / v23.y, color, alt);
		else if (v13.y != 0 && v23.y != 0)																											colorRow(window, Z_Buffers, p.x + (y - p.y) * v13.x / v13.y, p2.x + (y - p2.y) * v23.x / v23.y, y, z1 + z13 * (y - p.y) / v13.y, z2 + z23 * (y - p2.y) / v23.y, color, alt);
	}
}

void colorFace(SDL_Surface* window, int* Z_Buffers, listP* lp, int nbPoints, const Uint32 color, int alt) {

	listP* P = lp;
	point3 p = *P->p;

	point3* pSaved = createPoint(p.x, p.y, p.z);
	pSaved->p = p.p;
	point3* fpLeft;

	int nbPointsLeft = nbPoints - 1 - (int)((nbPoints-1) / 3);

	fpLeft = pSaved;

	for (int i = (int)((nbPoints - 1) / 3); i >= 0; i--) {

		if (P->next->next) colorTriangle(window, Z_Buffers, P->p->p, P->next->p->p, P->next->next->p->p, color, alt);
		else {
			colorTriangle(window, Z_Buffers, P->p->p, P->next->p->p, lp->p->p, color, alt);
			break;
		}

		P = P->next->next;
		p = *P->p;

		pSaved->next = createPoint(p.x, p.y, p.z);
		pSaved = pSaved->next;
		pSaved->p = p.p;
	}

	if (nbPointsLeft > 2) colorFace(window, Z_Buffers, fpLeft, nbPointsLeft, color, alt);

}

///////////////////////////////////////////////////////////

void RotateObj(obj* o, point3 rot) {

	float cosX = cos(rot.x);
	float sinX = sin(rot.x);
	float cosY = cos(rot.y);
	float sinY = sin(rot.y);
	float cosZ = cos(rot.z);
	float sinZ = sin(rot.z);

	point3* p = o->vertexes;
	point3 G = o->G;
	point3 v;

	for (int i = 0; i < o->nbVertexes; i++) {

		// Yaw
		v = sum(*p, G, -1);
		p->y = v.y * cosX - v.z * sinX + G.y;
		p->z = v.z * cosX + v.y * sinX + G.z;

		// Pitch
		v = sum(*p, G, -1);
		p->x = v.x * cosY + v.z * sinY + G.x;
		p->z = v.z * cosY - v.x * sinY + G.z;

		// Roll
		v = sum(*p, G, -1);
		p->x = v.x * cosZ - v.y * sinZ + G.x;
		p->y = v.y * cosZ + v.x * sinZ + G.y;

		p = p->next;
	}

	face* f = o->faces;
	point3 n;
	for (int i = 0; i < o->nbFaces; i++) {
		n = f->normale;

		// Yaw
		n.y = n.y * cosX - n.z * sinX;
		n.z = n.z * cosX + n.y * sinX;
		// Pitch
		n.x = n.x * cosY + n.z * sinY;
		n.z = n.z * cosY - n.x * sinY;
		// Roll
		n.x = n.x * cosZ - n.y * sinZ;
		n.y = n.y * cosZ + n.x * sinZ;

		f->normale = n;
		f = f->next;
	}
}

//SEULEMENT PREMIERE FACE
void ExtrudeFace(obj* o, point3 dir) {

	point3* pO = o->vertexes;
	face* f = o->faces;
	face* ff = f;
	listP* P = f->points;
	face* fTmp;

	point3 p;
	point3* newp;
	listP* listPf = f->points;
	listP* flistPf = listPf;
	point3** pTab = (point3**)calloc(f->nbPoints, sizeof(point3*));
	point3** pTab2 = (point3**)calloc(f->nbPoints, sizeof(point3*));

	int nbPoints = f->nbPoints;

	point3 G = o->G;
	point3 vfo;
	point3 nf;

	for (int i = 0; i < nbPoints; i++) {

		p = *P->p;

		pTab[i] = P->p;
		pTab2[i] = newp = createPoint(p.x + dir.x, p.y + dir.y, p.z + dir.z);

		newp->next = pO;
		pO = newp;

		if (i > 0) {

			listPf = listPf->next;

			// Face creating resulting of extrusion
			fTmp = createFace(f->color, 4, pTab[i], pTab[i - 1], pTab2[i - 1], pTab2[i]);

			// Correcting normal direction
			vfo = sum(G, fTmp->G, -1);
			nf = fTmp->normale;
			if (scalar(vfo, nf) > 0) fTmp->normale = scale(nf, -1);

			fTmp->next = f;
			f = fTmp;
		}
		listPf->p = newp;
		P = P->next;
	}

	fTmp = createFace(f->color, 4, pTab[0], pTab[nbPoints - 1], pTab2[nbPoints - 1], pTab2[0]);

	vfo = sum(G, fTmp->G, -1);
	nf = fTmp->normale;
	if (scalar(vfo, nf) > 0) fTmp->normale = scale(nf, -1);

	fTmp->next = f;
	f = fTmp;

	// Extruded face gets the moved new points
	ff->points = flistPf;
	o->vertexes = pO;
	o->nbVertexes += nbPoints;
	o->faces = f;
	o->nbFaces += nbPoints;

	free(pTab);
	free(pTab2);
}

///////////////////////////////////////////////////////////

void moveSpheres(listS* S, cam c) {

	sphere* s, *sbis;
	point3 v, * p, pssbis, pc, pcbis, ppc, cpos = c.pos;;
	listS* Sbis;
	float rad, radbis, playerDist;
	while (S) {

		s = S->s;
		p = s->pos;
		rad = s->radius;

		s->v.y -= 0.05;
		add(p, s->v, 0);

		// Ground bounce
		if (p->y - rad <= 0.f) {
			p->y = rad;
			s->v.y *= -0.75;
			s->v.x *= 0.98; // ground friction
			s->v.z *= 0.98; // ground friction
		}

		// Player Collision
		ppc = sum(*p, cpos, -1);
		ppc.y = 0;
		playerDist = norm(ppc);
			
		if (playerDist < 300 && abs(p->y - cpos.y) < 500) {
			p->x += (p->x - cpos.x) * sqrtf(300) / playerDist;
			p->z += (p->z - cpos.z) * sqrtf(300) / playerDist;
			add(&s->v, scale(ppc, 1.f / playerDist), 0);
		}

		// Symetrical Collison (handle the 2 colliders)
		Sbis = S->next;
		while (Sbis) {
			sbis = Sbis->s;
			radbis = sbis->radius;

			pssbis = sum(*sbis->pos, *p, -1);
			if (norm(pssbis) < rad + radbis) {
				pc = scale(pssbis, rad / (rad + radbis));
				pcbis = scale(pssbis, -radbis / (rad + radbis));

				add(&s->v, scale(pc, -0.05 * fabs(scalar(sbis->v, pcbis) / norm(pcbis)) / norm(pc)), 0);
				add(&sbis->v, scale(pcbis, -0.05 * fabs(scalar(s->v, pc) / norm(pc)) / norm(pcbis)), 0);
			}

			Sbis = Sbis->next;
		}

		S = S->next;
	}
}

// Memory Cleaning
void freeAll(listO* O, listS* S, listL* L) {

	freeListO(O);
	freeListS(S);
	freeListL(L);

}
void freeListO(listO* O) {
	if (!O) return;
	freeListO(O->next);
	freeObj(O->o);
	free(O);
}
void freeObj(obj* o) {

	freeFaces(o->faces);
	freePoints(o->vertexes);
	free(o);

}
void freeFaces(face* f) {

	if (!f) return;
	freeFaces(f->next);
	freeListP(f->points);
	free(f);

}
void freeListS(listS* S) {
	if (!S) return;
	freeListS(S->next);
	freeSphere(S->s);
	free(S);
}
void freeSphere(sphere* s) {
	if (s->pos) freePoints(s->pos);
	free(s);
}
void freeListP(listP* P) {

	if (!P) return;
	freeListP(P->next);
	free(P);

}
void freePoints(point3* p) {

	if (!p) return;
	freePoints(p->next);
	free(p);

}
void freeListL(listL* L) {
	if (!L) return;
	freeListL(L->next);
	freeLight(L->l);
	free(L);
}
void freeLight(light* l) {
	if (l->pos) freePoints(l->pos);
	free(l);
}