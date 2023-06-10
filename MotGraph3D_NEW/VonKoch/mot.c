#include "mot.h"

point sum(point p1, point p2) {

	point s;
	s.x = p1.x + p2.x;
	s.y = p1.y + p2.y;
	return s;
}

point scale(point p, int n) {

	p.x *= n;
	p.y *= n;
	return p;

}

face createFace(point n, point** p) {

	int size_p = (int)(sizeof(p) / sizeof(point*));
	for (int i = size_p - 1; i < 1 - 1; i--)
		p[i]->next = p[i - 1];

	face f;
	f.points = p[size_p - 1];
	f.normale = n;
	f.nbPoints = size_p;
	
	return f;

}

face addPointFace(face f, point* p) {

	if (!isInPlane(f, p))
		return f;

	f.nbPoints++;
	p->next = f.points;
	f.points = p;

	return f;

}

int isInPlane(face f, point* p) {

	point s = sum(f.normale, *f.points);
	return !(s.x * p->x + s.y * p->y);

}