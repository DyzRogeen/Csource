#include "graph.h"

// Object handling

camera* initCamera(point3 position, float lon, float lat, float UPP) {
	camera* cam = (camera*)calloc(1, sizeof(camera));
	cam->position = position;
	cam->lon = lon;
	cam->lat = lat;
	cam->UPP = UPP;
	cam->plane_dist = 4;

	return cam;
}

point3* createPoint(float x, float y, float z) {
	point3* p = (point3*)calloc(1, sizeof(point3));
	p->x = x;
	p->y = y;
	p->z = z;
	return p;
}

void freeCamera(camera* cam) {
	//free(cam->position);
	free(cam);
}


// Graphic methods

void updateProjectionPlane(camera* cam) {

	point3 pcam = cam->position;
	float lon = cam->lon, lat = cam->lat;
	float plane_dist = cam->plane_dist;

	// Calcul de la normale du plan camera
	cam->normal = (point3){
		cosf(lat) * cosf(lon),
		cosf(lat) * sinf(lon),
		sinf(lat)
	};

	// Calcul de l'origine du plan camera
	cam->origin = sum(pcam, scale(cam->normal, plane_dist));

	// Vecteurs orthogonaux du plan
	cam->u = (point3){
		cosf(lat) * cosf(lon + PI / 2),
		cosf(lat) * sinf(lon + PI / 2),
		0
	};
	cam->v = (point3){
		cosf(lat + PI / 2) * cosf(lon),
		cosf(lat + PI / 2) * sinf(lon),
		sinf(lat + PI / 2)
	};

}

point pojectPoint(camera cam, point3 p, int w2, int h2) {

	point3 v_p_cam = sub(cam.position, p);
	point3 origin = cam.origin;
	point3 normal = cam.normal;

	// Système d'équation paramétrique de la droite
	// X = px + vx * t
	// Y = py + vy * t
	// Z = pz + vz * t
	// 
	// Equation cartésienne du plan
	// (X - ox) * nx + (Y - oy) * ny + (Z - oz) * nz = 0
	//
	// On obtient :
	// (px + vx * t - ox) * nx + (py + vy * t - oy) * ny + (pz + vz * t - oz) * nz = 0
	// t * (vx * nx + vy * ny + vz * nz) = (ox - px) * nx + (oy - py) * ny + (oz - pz) * nz
	// t = ([ox - px] * nx + [oy - py] * ny + [oz - pz] * nz) / (vx * nx + vy * ny + vz * nz)
	float d = (v_p_cam.x * normal.x + v_p_cam.y * normal.y + v_p_cam.z * normal.z);

	// Si le vecteur est orthogonal à la normale, il ne rencontrera jamais le plan.
	if (d == 0) return (point) { -1, -1 };

	float t = ((origin.x - p.x) * normal.x + (origin.y - p.y) * normal.y + (origin.z - p.z) * normal.z) / d;

	// Si le coefficient est négatif, le point se trouve entre le plan et la caméra.
	// Si il est plus grand que 1, le point se trouve derrière la caméra
	if (t < 0 || t > 1) return (point) { -1, -1 };

	// Coordonées de la projection sur le plan
	float X = p.x + v_p_cam.x * t - origin.x;
	float Y = p.y + v_p_cam.y * t - origin.y;
	float Z = p.z + v_p_cam.z * t - origin.z;

	point3 u = cam.u, v = cam.v;

	// On veut déterminer les coordonées du point dans le repère du plan.
	// On utilise les vecteurs u et v du plan :
	// X = ux * m + vx * n	
	// Y = uy * m + vy * n	=> P = A * I avec P = [ X Y Z ]^T | A = [ u v ] | I = [ m n ]
	// Z = uz * m +	vz * n	
	// 
	// 3 equations pour 2 inconnues mais il existe bien une unique solution.
	// On réduit les dimensions avec une matrice de Gram
	// A^T * P = (A^T * A) * I
	// Avec A^T * A = [ u*u u*v ] = [ u*u  0  ] (car u et v sont orthogonaux)
	//				  [ v*u v*v ] = [  0  v*v ]
	// On a alors :
	// u*u * m = P * u => m = P * u / u*u => m = (X * u.x + Y * u.y + Z * u.z) / (u.x * u.x + u.y * u.y + u.z * u.z)
	// v*v * n = P * v => n = P * v / v*v => n = (X * v.x + Y * v.y + Z * v.z) / (v.x * v.x + v.y * v.y + v.z * v.z)

	float m = (X * u.x + Y * u.y) / (u.x * u.x + u.y * u.y); // Avec uz nul.
	float n = Z / v.z; // n ne dépent que de Z et vz car il n'y a plus de notion de profondeur en 2D, on simplifie.

	float UPP = cam.UPP;

	// On recentre le point sur l'écran avec les demi largeur et hauteur
	return (point) { w2 + m / UPP, h2 - n / UPP };
}

void drawGrid(SDL_Surface* s, camera cam) {

	float w = s->w, h = s->h;
	int w2 = w / 2, h2 = h / 2;

	point pO = pojectPoint(cam, (point3) { 0, 0, 0 }, w2, h2);
	//point pX = pojectPoint(cam, (point3) { 50, 0, 0 }, w2, h2);
	//point pY = pojectPoint(cam, (point3) { 0, 50, 0 }, w2, h2);
	point pZ = pojectPoint(cam, (point3) { 0, 0, 50 }, w2, h2);

	point pTmp = pO;

	//if (!(pTmp.x == pX.x && pTmp.y == pX.y)) {
	//	extendVectorToEdges(&pTmp.x, &pTmp.y, &pX.x, &pX.y, w, h);
	//	drawline(s, pTmp, pX, C_RED);
	//	pTmp = pO;
	//}
	//if (!(pTmp.x == pY.x && pTmp.y == pY.y)) {
	//	extendVectorToEdges(&pTmp.x, &pTmp.y, &pY.x, &pY.y, w, h);
	//	drawline(s, pTmp, pY, C_BLUE);
	//	pTmp = pO;
	//}
	if (!(pTmp.x == pZ.x && pTmp.y == pZ.y)) {
		extendVectorToEdges(&pTmp.x, &pTmp.y, &pZ.x, &pZ.y, w, h, 0);
		drawline(s, pTmp, pZ, C_GREEN);
	}
	
	// Calcul de l'horizon.
	// D'après projectPoint le rapport t tend vers 1 si px ou py tendent vers +inf
	// On obtient alors Z = pcam.z - o.z et donc y = h/2 - (pcam.z - o.z) / (v.z * UPP)
	int horizon = h2 - (float)(cam.position.z - cam.origin.z) / (cam.v.z * cam.UPP);

	point p1, p2;
	float factor = 10;
	for (float i = -20; i <= 20; i++) {
		p1 = pojectPoint(cam, (point3) { factor * i, 0, 0 }, w2, h2);
		p2 = pojectPoint(cam, (point3) { factor * i, 50, 0 }, w2, h2);
		extendAndDrawline(s, p1, p2, i == 0 ? C_BLUE : C_LGREY, h, w, horizon);
	}
	for (int i = -20; i <= 20; i++) {
		p1 = pojectPoint(cam, (point3) { 0, factor* i, 0 }, w2, h2);
		p2 = pojectPoint(cam, (point3) { 50, factor* i, 0 }, w2, h2);
		extendAndDrawline(s, p1, p2, i == 0 ? C_RED : C_LGREY, h, w, horizon);
	}

	/*for (int i = -10; i <= 10; i++) {
		p1 = pojectPoint(cam, (point3) { factor* i, 0, 0 }, w2, h2);
		p2 = pojectPoint(cam, (point3) { factor* i, 0, 50 }, w2, h2);
		extendAndDrawline(s, p1, p2, i == 0 ? C_GREEN : C_LGREY, h, w);
	}
	for (int i = -10; i <= 10; i++) {
		if (i == 0) continue;
		p1 = pojectPoint(cam, (point3) { 0, 0, factor* i }, w2, h2);
		p2 = pojectPoint(cam, (point3) { 50, 0, factor* i }, w2, h2);
		extendAndDrawline(s, p1, p2, C_LGREY, h, w);
	}

	for (int i = -10; i <= 10; i++) {
		if (i == 0) continue;
		p1 = pojectPoint(cam, (point3) { 0, factor* i, 0 }, w2, h2);
		p2 = pojectPoint(cam, (point3) { 0, factor* i, 50 }, w2, h2);
		extendAndDrawline(s, p1, p2, C_LGREY, h, w);
	}
	for (int i = -10; i <= 10; i++) {
		if (i == 0) continue;
		p1 = pojectPoint(cam, (point3) { 0, 0, factor* i }, w2, h2);
		p2 = pojectPoint(cam, (point3) { 0, 50, factor* i }, w2, h2);
		extendAndDrawline(s, p1, p2, C_LGREY, h, w);
	}*/

	// Ancienne méthode
	/*float UPP = cam.UPP;
	float plane_dist = cam.plane_dist;

	point3 origin = cam.origin;
	point3 normal = cam.normal;
	point3 u = cam.u, v = cam.v;

	// On veut trouver l'intersection entre les axes et les 4 plans qui bordent l'espace visuel (écran dans l'espace).
	// Il nous faut d'abord trouver les équations qui définissent ces plans en prenant la caméra comme origine.

	//// Demi longueur et largeur de l'écran.
	//u = scale(u, w);
	//v = scale(v, h);
	
	float angle_phi = atanf(w * UPP / plane_dist);
	float angle_psi = atanf(h * UPP / plane_dist);

	// Rotation de la normale pour obtenir les normales des plans Nord, Sud, Est et Ouest.
	point3 normal_n = sum(scale(v, -cosf(angle_psi)), scale(normal, sinf(angle_psi)));
	point3 normal_s = sum(scale(v,  cosf(angle_psi)), scale(normal, sinf(angle_psi)));
	point3 normal_e = sum(scale(u, -cosf(angle_phi)), scale(normal, sinf(angle_phi)));
	point3 normal_w = sum(scale(u,  cosf(angle_phi)), scale(normal, sinf(angle_phi)));

	// Equations :
	// Axe X : X = ox + (oy * ny + oz * nz) / nx
	// Axe Y : Y = oy + (ox * nx + oz * nz) / ny
	// Axe Z : Z = oz + (ox * nx + oy * ny) / nz

	float X1 = origin.x + (origin.y * normal_e.y + origin.z	* normal_e.z) / normal_e.x;
	float X2 = origin.x + (origin.y * normal_w.y + origin.z	* normal_w.z) / normal_w.x;

	float Y1 = origin.y + (origin.x * normal_e.x + origin.z * normal_e.z) / normal_e.y;
	float Y2 = origin.y + (origin.x * normal_w.x + origin.z * normal_w.z) / normal_w.y;

	float Z1 = origin.z + (origin.x * normal_n.x + origin.y * normal_n.y) / normal_n.z;
	float Z2 = origin.z + (origin.x * normal_s.x + origin.y * normal_s.y) / normal_s.z;

	point pX1 = pojectPoint(cam, (point3) { X1, 0, 0 }, w2, h2);
	point pX2 = pojectPoint(cam, (point3) { X2, 0, 0 }, w2, h2);

	point pY1 = pojectPoint(cam, (point3) { 0, Y1, 0 }, w2, h2);
	point pY2 = pojectPoint(cam, (point3) { 0, Y2, 0 }, w2, h2);

	point pZ1 = pojectPoint(cam, (point3) { 0, 0, Z1 }, w2, h2);
	point pZ2 = pojectPoint(cam, (point3) { 0, 0, Z2 }, w2, h2);

	drawline(s, pX1, pX2, C_WHITE);
	drawline(s, pY1, pY2, C_WHITE);
	drawline(s, pZ1, pZ2, C_WHITE);*/

}

void drawline(SDL_Surface* s, point p1, point p2, Uint32 color) {

	int w = s->w, h = s->h;
	Uint32* pxls = s->pixels;

	int p1x = p1.x, p2x = p2.x;
	int p1y = p1.y, p2y = p2.y;

	float vx = p2x - p1x;
	float vy = p2y - p1y;

	// TODO corriger car pointillés
	float norm = sqrtf(min(vx * vx + vy * vy, w * w + h * h));

	vx /= norm;
	vy /= norm;

	int X, Y;

	for (int i = 0; i < norm; i++) {
		X = p1x + vx * i;
		if (X < 0 || X >= w) continue;
		Y = p1y + vy * i;
		if (Y < 0 || Y >= h) continue;
		
		pxls[X + Y * w] = color;
	}

}

void extendVectorToEdges(int* p1_x, int* p1_y, int* p2_x, int* p2_y, int w, int h, int horizon) {
	float v_p1_x = *p1_x, v_p2_x = *p2_x, v_p1_y = *p1_y, v_p2_y = *p2_y;

	float vx = v_p2_x - v_p1_x;
	float vy = v_p2_y - v_p1_y;

	horizon = max(min(horizon, h - 1), 0);

	int y1Tmp, y2Tmp;
	// On "colle" p1 sur l'axe (X;Y)
	// On compare pour éviter un dénominateur nul et préférer un point plus proche de (0;0)
	if (abs(vx) > abs(vy)) {

		// (y1Tmp = v_p1_y - v_p1_x * vy / vx) >= horizon && (y2Tmp = v_p2_y + (w - 1 - v_p2_x) * vy / vx) >= horizon
		
		// px + vx * n = 0 => n = -px / vx
		// py + vy * n = Y
		if ((y1Tmp = v_p1_y - v_p1_x * vy / vx) >= horizon) {
			*p1_x = 0;
			*p1_y = y1Tmp;
		}
		else {
			*p1_x = v_p1_x + (horizon - v_p1_y) * vx / vy;
			*p1_y = horizon;
		}
		

		// px + vx * n = w-1 => n = (w - 1 - px) / vx
		// py + vy * n = Y
		if ((y2Tmp = v_p2_y + (w - 1 - v_p2_x) * vy / vx) >= horizon) {
			*p2_x = w - 1;
			*p2_y = y2Tmp;
		}
		else {
			*p2_x = v_p2_x + (horizon - v_p2_y) * vx / vy;
			*p2_y = horizon;
		}
		
	}
	else {
		*p1_x = v_p1_x + (horizon - v_p1_y) * vx / vy;
		*p1_y = horizon;

		*p2_x = v_p2_x + (h - 1 - v_p2_y) * vx / vy;
		*p2_y = h - 1;
	}

}

void extendAndDrawline(SDL_Surface *s, point p1, point p2, Uint32 color, int h, int w, int horizon) {
	if ((p1.x == -1 && p1.y == -1 || p2.x == -1 && p2.y == -1) || p1.x == p2.x && p1.y == p2.y) return;
	extendVectorToEdges(&p1.x, &p1.y, &p2.x, &p2.y, w, h, horizon);
	drawline(s, p1, p2, color);
}

// Vector methods

point3 sum(point3 p1, point3 p2) {
	return (point3) { p1.x + p2.x, p1.y + p2.y, p1.z + p2.z };
}
point3 sub(point3 p1, point3 p2) {
	return (point3) { p1.x - p2.x, p1.y - p2.y, p1.z - p2.z };
}
point3 scale(point3 p, float scale) {
	return (point3) { p.x* scale, p.y* scale, p.z* scale };
}