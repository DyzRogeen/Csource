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

	if (vcf.x * nf.x + vcf.y * nf.y + vcf.z * nf.z < 0) {
		listP* p = f.points;
		for (int i = 0; i < f.nbPoints; i++) {
			if (p->p->p.display) return 1;
			p = p->next;
		}
	}
	return 0;
}

point3 getUnitV(point3 v) {

	return scale(v, 1/norm(v));

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

	p1.x += p2.x * diff;
	p1.y += p2.y * diff;

	return p1;
}

float norm2(point p) {
	return sqrt(p.x*p.x + p.y*p.y);
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

float scalar(point3 p1, point3 p2) {
	return p1.x * p2.x + p1.y * p2.y + p1.z * p2.z;
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

// Edges and Faces
edge* createEdge(point3* p1, point3* p2) {
	edge* e = (edge*)calloc(1,sizeof(edge));
	e->points[0] = p1;
	e->points[1] = p2;
	e->next = NULL;

	return e;
}

face* createFace(Uint8 color[3], int nbPoints, ...) {

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

	f->color[0] = color[0];
	f->color[1] = color[1];
	f->color[2] = color[2];

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

// Object
obj* createObj3D(face* f, edge* e, point3* p, int nbFaces, int nbEdges, int nbVertexes, int isStatic) {

	obj* o = (obj*)calloc(1, sizeof(obj));

	o->nbFaces = nbFaces;
	o->nbEdges = nbEdges;
	o->nbVertexes = nbVertexes;
	o->isStatic = isStatic;

	o->faces = f;
	o->edges = e;
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

obj* createCube(point3 pos, point3 rot, float size, Uint8 color[3], int isStatic) {

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

	obj* o = createObj3D(*F, NULL, *P, 6, 0, 8, isStatic);
	RotateObj(o, rot);

	return o;
}

obj* createRect(point3 p1, point3 p2, Uint8 color[3]) {



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
	c.dist = 15;
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
light initLight(point3 pos, float intensity, Uint8 color[3]) {

	light l;
	l.pos = pos;
	l.intensity = intensity;
	l.color[0] = color[0];
	l.color[1] = color[1];
	l.color[2] = color[2];

	return l;

}

void addLight(face f, cam c, light l, Uint8 color[3]) {

	point3 vlf = sum(f.G, l.pos, -1);
	point3 vfc = getUnitV(sum(c.pos, f.G, -1));
	point3 normal = f.normale;

	// Getting direction of the reflected light vector on the face
	point3 vr = getUnitV(sum(getUnitV(f.normale), getUnitV(vlf), 0));

	float nlf = norm(vlf) / 10;

	// Scalar product with the vector face-camera with all the other bullshit tweaked on ass makes the brightness coef
	float coef = (scalar(vr,vfc)*4 - scalar(normal,vlf)/4) * l.intensity / (nlf * nlf);

	Uint8* cf = f.color;
	Uint8* cl = l.color;

	color[0] = (coef < 0) ? cf[0] : (cf[0] + cl[0] * coef) / (coef + 1);
	color[2] = (coef < 0) ? cf[2] : (cf[2] + cl[2] * coef) / (coef + 1);
	color[1] = (coef < 0) ? cf[1] : (cf[1] + cl[1] * coef) / (coef + 1);

}

// Render
void pointsTo2dProjection(int screenW, int screenH, point3* p, int nbPoints, cam c) {

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
	float n,k;

	// Intersection coordinates
	float X, Y, Z;

	for (int i = 0; i < nbPoints; i++) {

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

		if (k > 1 || k < 0) {
			p->p.display = 0;
			p = p->next;
			continue;
		}

		// P = v*k + p
		X = (pos.x - p->x) * k + p->x - pcam.x;
		Y = (pos.y - p->y) * k + p->y - pcam.y;
		Z = (pos.z - p->z) * k + p->z - pcam.z;

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
		float den;
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

		p->p.depth = norm(vpc);
		
		p->p.display = 1;
		p = p->next;
	}

}

void displayObj(SDL_Surface* window, int* Z_Buffers, obj o, cam c, light l, int camHasMoved, int alt) {

	pointsTo2dProjection(window->w/2, window->h/2, o.vertexes, o.nbVertexes, c);

	/*edge* e = o.edges;
	point p1, p2;

	for (int i = 0; i < o.nbEdges; i++) {
		p1 = e->points[0]->p;
		p2 = e->points[1]->p;
		if (p1.display && p2.display) _SDL_DrawLine(window, p1.x, p1.y, p2.x, p2.y, 255, 255, 255);
		e = e->next;
	}*/

	face* f = o.faces;
	Uint8 color[3];

	for (int i = 0; i < o.nbFaces; i++) {

		if (camHasMoved) {
			if (f->display = checkFaceDisplay(*f, c)) {
				addLight(*f, c, l, color);
				colorFace(window, Z_Buffers, f->points, f->nbPoints, SDL_MapRGB(window->format, color[0], color[1], color[2]), alt);
			}
		}
		else {
			if (f->display) {
				addLight(*f, c, l, color);
				colorFace(window, Z_Buffers, f->points, f->nbPoints, SDL_MapRGB(window->format, color[0], color[1], color[2]), alt);
			}
		}
		
		f = f->next;
	}
	
}

void colorRow(SDL_Surface* window, int* Z_Buffers, int x1, int x2, int y,  int z1, int z2, const Uint32 color, int alt) {

	int w = window->w;
	if (x1 == x2 || (x1 < 0 || x1 >= w) && (x2 < 0 || x2 >= w)) return;

	int h = window->h;

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

// Memory Cleaning
void freeAll(listO* O) {

	if (!O) return;
	freeAll(O->next);
	freeObj(O->o);
	free(O);

}
void freeObj(obj* o) {

	freeFaces(o->faces);
	freeEdges(o->edges);
	freePoints(o->vertexes);
	free(o);

}
void freeFaces(face* f) {

	if (!f) return;
	freeFaces(f->next);
	freeListP(f->points);
	free(f);

}
void freeEdges(edge* e) {

	if (!e) return;
	freeEdges(e->next);
	free(e);

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