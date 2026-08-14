#include "graph.h"

#define TUL_SIZE 1024

float asin_table[TUL_SIZE];
float sin_table[TUL_SIZE];
float atan_table[TUL_SIZE];

// Planétaire
astre* createAstre(point3 position, point3 velocity, point3 acceleration, float mass, float massOrder, float radius, float radiusOrder, float tRotation, float lonRotAxis, float latRotAxis, astreType type, char* texture, char name[100]) {
	astre* a = (astre*)calloc(1, sizeof(astre));
	a->position = position;
	a->velocity = velocity;
	a->acceleration = acceleration;

	a->mass = mass;
	a->massOrder = massOrder; // kg x10^order
	a->radius = radius;
	a->radiusOrder = radiusOrder; // km x10^order

	a->tRotation = tRotation; // jours
	a->lonRotAxis = lonRotAxis;
	a->latRotAxis = latRotAxis;

	a->type = type;
	a->name = name;

	if (!(a->texture = IMG_Load(texture))) {
		printf("Unable to open jpg file.\n");
		a->texture = NULL;
	}

	return a;
}
list* createList(astre* astre) {
	list* l = (list*)calloc(1, sizeof(list));
	l->p_astre = astre;
	l->next = NULL;
	l->prec = NULL;
	return l;
}
void addAstre(list** l, astre* astre) {
	if (!l) return;
	if (!*l) {
		*l = createList(astre);
		return;
	}

	list* ltmp = *l;
	*l = createList(astre);
	ltmp->prec = *l;
	(*l)->next = ltmp;
}

// Camera and movement
cam* createCam(point3 pos, float lat, float lon, float zoom, float window_ratio) {
	cam* c = (cam*)calloc(1, sizeof(cam));
	c->pos = pos;
	c->lat = lat;
	c->lon = lon;
	c->d_plan = zoom;
	c->window_ratio = window_ratio;

	c->normale = polaireToCartesien(lat, lon);
	c->Vx = polaireToCartesien(lat, lon + PI_2);
	c->Vy = polaireToCartesien(lat + PI_2, lon);

	return c;
}
void moveCam(cam* c, point3 movement) {
	c->pos = sum3(c->pos, movement, 1);
}
void rotateCam(cam* c, float d_lat, float d_lon) {

	float tmp;
	float lat = (tmp = c->lat + d_lat) > PI_2 ? PI_2 : tmp < -PI_2 ? -PI_2 : tmp;
	float lon = (tmp = c->lon + d_lon) > PI2 ? tmp - PI2 : tmp < 0 ? tmp + PI2 : tmp;

	c->normale = polaireToCartesien(lat, lon);
	c->Vx = polaireToCartesien(0, lon + PI_2);
	c->Vy = polaireToCartesien(lat + PI_2, lon);
	c->lat = lat;
	c->lon = lon;

	//printf("[ %.2f ; %.2f ] => [ %.2f ; %.2f ; %.2f ] | [ %.2f ; %.2f ; %.2f ] | [ %.2f ; %.2f ; %.2f ]\n", lon, lat, c->normale.x, c->normale.y, c->normale.z, c->Vx.x, c->Vx.y, c->Vx.z, c->Vy.x, c->Vy.y, c->Vy.z);
}
int goToAstre(cam* c, astre* a) {
	point3 cpos = c->pos, normale = c->normale;
	point3 apos = a->position;
	point3 v_ca = sum3(apos, cpos, -1);
	float dist = norm3(v_ca);
	float radius = a->radius * pow(10, a->radiusOrder);


	v_ca = scale3(v_ca, 1.f / dist);

	// Rotation
	float lat_v = asinf(v_ca.z), lat_c = c->lat;
	float lon_v = atanf(v_ca.y / v_ca.x), lon_c = c->lon > PI ? c->lon - PI2 : c->lon;
	float d_rot = norm2((point2) { lat_v - lat_c, lon_v - lon_c });

	if (d_rot > 0.005f) {
		float pas = sqrtf(d_rot) * .1f + 0.05f; // A faire varier selon l'écart
		rotateCam(c, (lat_v - lat_c) * pas, (lon_v - lon_c) * pas);
	}
	else {
		rotateCam(c, (lat_v - lat_c), (lon_v - lon_c));
		if (dist < 3 * radius) return 1; // On est assez proche
	}
	

	// Translation
	if (dist > 3 * radius) {
		float distr = dist - 3 * radius;
		float speed = (distr / 25 + sqrtf(distr) + 80) * (PI / 4 - fabs(lon_v - lon_c));
		moveCam(c, (point3) { normale.x* speed, normale.y * speed, normale.z* speed });
	}
	

	return 0;

	// Idée : Reculer la caméra en l'orientant progressivement vers l'astre jusqu'à ce que le produit scalaire soit assez grand, puis avancer la caméra jusqu'à destination

}

// Rendering
point2 projectPoint(cam* c, point3 p, int w, int h) {
	point3 pcam = scale3(c->pos, 1.f / 1000.f), normale = c->normale;
	p = scale3(p, 1.f / 1000.f);
	point3 vpc = sum3(pcam, p, -1);
	point3 pplane = sum3(pcam, scale3(normale, c->d_plan), 1);
	float window_ratio = c->window_ratio;
	
	float d = vpc.x * normale.x + vpc.y * normale.y + vpc.z * normale.z;

	// Si d est nul alors le point ne sera jamais projeté sur le plan caméra.
	if (d == 0) return (point2) { -1, -1 };

	float k = ((pplane.x - p.x) * normale.x + (pplane.y - p.y) * normale.y + (pplane.z - p.z) * normale.z) / d;

	// Si k < 0 le point est entre la caméra et le plan, si k > 1 le point est derrière la caméra.
	if (k < 0 || k > 1) return (point2) { -1, -1 };

	float X = p.x + vpc.x * k - pplane.x;
	float Y = p.y + vpc.y * k - pplane.y;
	float Z = p.z + vpc.z * k - pplane.z;


	point3 V1 = scale3(c->Vx, window_ratio);
	point3 V2 = scale3(c->Vy, window_ratio);

	float x = (X * V1.x + Y * V1.y) / (V1.x * V1.x + V1.y * V1.y);
	float y = Z / V2.z;

	//printf("==========================\n%f : XYZ [ %f ; %f ; %f ] : p [ %f ; %f ; %f ] : vpc [ %f ; %f ; %f ] : pplane [ %f ; %f ; %f ] : V1 [ %f ; %f ; %f ] : V2 [ %f ; %f ; %f ] : xy [ %f ; %f ]\n", k, X, Y, Z, p.x, p.y, p.z, vpc.x, vpc.y, vpc.z, pplane.x, pplane.y, pplane.z, V1.x, V1.y, V1.z, V2.x, V2.y, V2.z, x, y);

	return (point2) { w / 2.f + x, h / 2 - y };
}
void renderAstres(SDL_Surface* surface, cam* c, list* l, int nb_astre, float dT) {

	// TODO : Gérer l'ordre de render
	astre* orderedAstre = malloc(nb_astre * sizeof(astre));

	astre* a;

	while (l) {
		a = l->p_astre;
		a->lonRotAxis += a->tRotation * dT * PI2;

		renderAstre(surface, c, a);
		l = l->next;
	}

	free(orderedAstre);
}
void renderAstre(SDL_Surface* surface, cam* c, astre* a) {

	SDL_Surface* texture = a->texture;
	if (!texture) return;

	int w = surface->w, h = surface->h, w_texture = texture->w, h_texture = texture->h;
	Uint32* pxls = surface->pixels;
	Uint8* pxls_texture = texture->pixels, * pxl;


	point3 a_pos = a->position;
	point2 p_screen = projectPoint(c, a_pos, w, h);
	if (p_screen.x == -1 && p_screen.y == -1) return;

	float p_x = p_screen.x;
	float p_y = p_screen.y;
	
	point3 v_ac = sum3(c->pos, a_pos, -1);
	float dist = norm3(v_ac);
	float radius = a->radius * pow(10, a->radiusOrder);

	int screen_radius = radius * c->d_plan / (dist * c->window_ratio);

	if (screen_radius <= 0) return;

	// TODO : rotation selon l'axe astre caméra
	v_ac = scale3(v_ac, 1.f / dist);
	float lat, lat1, init_lat = asinf(v_ac.z) - a->latRotAxis;
	float lon, lon1, init_lon = atanf(v_ac.y / v_ac.x);
	if (v_ac.x < 0) init_lon -= PI;

	// Vecteur pour la rotation latitudinale
	point3 v_rot_lat = { v_ac.y, -v_ac.x, 0 };

	float init_cos_lon = cosf(init_lon), init_sin_lon = sinf(init_lon);
	float init_cos_lat = cosf(init_lat), init_sin_lat = sinf(init_lat);

	//if (v_ac.x > 0) init_lon = init_lon + PI;
	//init_lon *= (1.f + cosf(2.f * asinf(v_ac.z) + PI)) / 2.f;

	//printf("[ %.2f ; %.2f ] et [ %.2f ; %.2f ; %.2f ]\n", init_lon, init_lat, c->pos.x, c->pos.y, c->pos.z);

	point3 a_dir = unit3(a_pos);

	// Problèmes
	// - init_lon change brusquamment quand on passe au dessus du pôle
	// - translation latitudinale seulement sur l'axe Y
	// - faire tourner selon l'axe ac en fonction de la longitude de c
	// - Eclairage du soleil -> à faire selon coordonnées indépendantes

	// Rotation d'un point
	//		[ cos(ϕ) * cos(θ) ]   [ x ]
	// P =  [ cos(ϕ) * sin(θ) ] = [ y ]
	//		[	   sin(ϕ)	  ]   [ z ]
	// 
	// ϕ = asin(z) et θ = acos(x / cos(asin(z))
	// 
	//		[ cos(a)   0   sin(a) ] [ x ]
	// Ry = [   0      1     0    ] [ y ]
	//		[ sin(a)   0  -cos(a) ] [ z ]
	
	float X, Y, Z, den;
	int x_texture, y_texture;
	int x_start = p_x - screen_radius, x_end = p_x + screen_radius;
	int y_start = p_y - screen_radius, y_end = p_y + screen_radius;
	for (float y = y_start; y <= y_end; y++) {

		if (y <= 0 || y > h) continue;

		Z = (float)(y - p_y) / screen_radius;

		for (float x = x_start; x <= x_end; x++) {

			if (screen_radius < norm2((point2) { x - p_x, y - p_y })) continue;
			if (x <= 0 || x >= w) continue;

			X = (float)(x - p_x) / screen_radius;
			Y = sqrtf(1 - X * X - Z * Z);

			//float zrotX = X;
			//float zrotY = Y;
			//float zrotZ = Z;

			//// Rotation autour de Z
			//float zrotX = X * init_sin_lon + Y * init_cos_lon;
			//float zrotY = X * init_cos_lon - Y * init_sin_lon;
			//float zrotZ = Z;

			// Rotation de Rodrigues
			// On recherche à effectuer la rotation d'un vecteur U sur un axe défini par le vecteur quelconque N
			// La partie invariante par rotation de U sera (U . N)N colinéaire à N
			// La partie qui effectura la rotation sera donc W = U - (U . N)N
			// Le vecteur orthogonal à W par rapport à N sera W^N
			// Donc pour une rotation de θ on aura W*cosθ + (W^N)*sinθ
			// Donc l'image de U dans la rotation sera V = W*cosθ + (W^N)*sinθ + (U . N)N =  (U - (U . N)N)*cosθ + ((U - (U . N)N)^N)*sinθ + (U . N)N
			// Soit V = U*cosθ + (1 - cosθ)*(U . N)N + (U^N)*sinθ
			// 
			// Vx = X * cosθ + (1 - cosθ) * (Nx * Nx * X + Nx * Ny * Y + Nx * Nz * Z) + sinθ * (Ny * Z - Nz * Y);
			// Vx = Y * cosθ + (1 - cosθ) * (Ny * Nx * X + Ny * Ny * Y + Ny * Nz * Z) + sinθ * (Nz * X - Nx * Z);
			// Vx = Z * cosθ + (1 - cosθ) * (Nz * Nx * X + Nz * Ny * Y + Nz * Nz * Z) + sinθ * (Nx * Y - Ny * X);
			// 
			// Donc
			// 
			//float nrotX = zrotX * init_cos_lat + (1 - init_cos_lat) * (v_rot_lat.x * v_rot_lat.x * zrotX + v_rot_lat.x * v_rot_lat.y * zrotY + v_rot_lat.x * v_rot_lat.z * zrotZ) + init_sin_lat * (v_rot_lat.y * zrotZ - v_rot_lat.z * zrotY);
			//float nrotY = zrotY * init_cos_lat + (1 - init_cos_lat) * (v_rot_lat.y * v_rot_lat.x * zrotX + v_rot_lat.y * v_rot_lat.y * zrotY + v_rot_lat.y * v_rot_lat.z * zrotZ) + init_sin_lat * (v_rot_lat.z * zrotX - v_rot_lat.x * zrotZ);
			//float nrotZ = zrotZ * init_cos_lat + (1 - init_cos_lat) * (v_rot_lat.z * v_rot_lat.x * zrotX + v_rot_lat.z * v_rot_lat.y * zrotY + v_rot_lat.z * v_rot_lat.z * zrotZ) + init_sin_lat * (v_rot_lat.x * zrotY - v_rot_lat.y * zrotX);
			//
			// Avec Nz = 0 :
			//float nrotX = zrotX * init_cos_lat + (1 - init_cos_lat) * (v_rot_lat.x * v_rot_lat.x * zrotX + v_rot_lat.x * v_rot_lat.y * zrotY) + init_sin_lat * v_rot_lat.y * zrotZ;
			//float nrotY = zrotY * init_cos_lat + (1 - init_cos_lat) * (v_rot_lat.y * v_rot_lat.x * zrotX + v_rot_lat.y * v_rot_lat.y * zrotY) - init_sin_lat * v_rot_lat.x * zrotZ;
			//float nrotZ = zrotZ * init_cos_lat + init_sin_lat * (v_rot_lat.x * zrotY - v_rot_lat.y * zrotX);

			//float nrotX = X * init_cos_lat + (1 - init_cos_lat) * (v_rot_lat.x * v_rot_lat.x * X + v_rot_lat.x * v_rot_lat.y * Y) + init_sin_lat * v_rot_lat.y * Z;
			//float nrotY = Y * init_cos_lat + (1 - init_cos_lat) * (v_rot_lat.y * v_rot_lat.x * X + v_rot_lat.y * v_rot_lat.y * Y) - init_sin_lat * v_rot_lat.x * Z;
			//float nrotZ = Z * init_cos_lat + init_sin_lat * (v_rot_lat.x * Y - v_rot_lat.y * X);

			// v_ac.x   v_ac.y
			// v_ac.y  -v_ac.x
			// v_ac.z     0
			// Rotation autour de Z
			v_rot_lat = (point3){ v_ac.x * v_ac.z, v_ac.z * v_ac.y ,v_ac.y * v_ac.y + v_ac.x * v_ac.x };
			float zrotX = X * init_cos_lon + (1.f - init_cos_lon) * (v_rot_lat.x * v_rot_lat.x * X + v_rot_lat.x * v_rot_lat.y * Y + v_rot_lat.x * v_rot_lat.z * Z) + init_sin_lon * (v_rot_lat.y * Z - v_rot_lat.z * Y);
			float zrotY = Y * init_cos_lon + (1.f - init_cos_lon) * (v_rot_lat.y * v_rot_lat.x * X + v_rot_lat.y * v_rot_lat.y * Y + v_rot_lat.y * v_rot_lat.z * Z) + init_sin_lon * (v_rot_lat.z * X - v_rot_lat.x * Z);
			float zrotZ = Z * init_cos_lon + (1.f - init_cos_lon) * (v_rot_lat.z * v_rot_lat.x * X + v_rot_lat.z * v_rot_lat.y * Y + v_rot_lat.z * v_rot_lat.z * Z) + init_sin_lon * (v_rot_lat.x * Y - v_rot_lat.y * X);

			v_rot_lat = (point3){ -v_ac.x, -v_ac.y, 0.f };
			////v_rot_lat = (point3){ 1, -1, 0 };
			float nrotX = zrotX * init_cos_lat + (1.f - init_cos_lat) * (v_rot_lat.x * v_rot_lat.x * zrotX + v_rot_lat.x * v_rot_lat.y * zrotY + v_rot_lat.x * v_rot_lat.z * zrotZ) + init_sin_lat * (v_rot_lat.y * zrotZ - v_rot_lat.z * zrotY);
			float nrotY = zrotY * init_cos_lat + (1.f - init_cos_lat) * (v_rot_lat.y * v_rot_lat.x * zrotX + v_rot_lat.y * v_rot_lat.y * zrotY + v_rot_lat.y * v_rot_lat.z * zrotZ) + init_sin_lat * (v_rot_lat.z * zrotX - v_rot_lat.x * zrotZ);
			float nrotZ = zrotZ * init_cos_lat + (1.f - init_cos_lat) * (v_rot_lat.z * v_rot_lat.x * zrotX + v_rot_lat.z * v_rot_lat.y * zrotY + v_rot_lat.z * v_rot_lat.z * zrotZ) + init_sin_lat * (v_rot_lat.x * zrotY - v_rot_lat.y * zrotX);

			//// Rotation autour de Z
			//float zrotX = nrotX * init_sin_lon + nrotY * init_cos_lon;
			//float zrotY = nrotX * init_cos_lon - nrotY * init_sin_lon;
			//float zrotZ = nrotZ;

			//// Rotation autour de Z
			//float zrotX = X * init_sin_lon + Y * init_cos_lon;
			//float zrotY = X * init_cos_lon - Y * init_sin_lon;
			//float zrotZ = Z;
			//// Rotation autour de Y
			//float yrotX =  zrotX * init_cos_lat + zrotZ * init_sin_lat;
			//float yrotY =  zrotY;
			//float yrotZ = -zrotX * init_sin_lat + zrotZ * init_cos_lat;

			//// Rotation autour de Y
			//float yrotX =  X * init_cos_lat + Z * init_sin_lat;
			//float yrotY =  Y;
			//float yrotZ = -X * init_sin_lat + Z * init_cos_lat;
			//// Rotation autour de Z
			//float zrotX = yrotX * init_sin_lon + yrotY * init_cos_lon;
			//float zrotY = yrotX * init_cos_lon - yrotY * init_sin_lon;
			//float zrotZ = yrotZ;

			lat = fast_asin(nrotZ);
			lon = fast_atan(nrotY / nrotX) + PI_2 * (nrotX < 0 ? -1 : 1);

			x_texture = w_texture - (lon/* - a->lonRotAxis*/) * w_texture / PI2; // TODO : inclure lonRotAxis dans init_lon ?
			y_texture = lat * h_texture / PI + h_texture / 2.f;

			x_texture = (x_texture + w_texture) % w_texture;
			y_texture = (y_texture + h_texture) % h_texture;
			pxl = pxls_texture + (x_texture + y_texture * w_texture) * 3;

			pxls[(int)x + (int)(y * w)] = 0xFF << 24 | pxl[0] << 16 | pxl[1] << 8 | pxl[2];

			continue;


			point3 pxl_dir = unit3((point3) { 0,0,0 });
			//point3 pxl_dir = unit3((point3) { zrotX , zrotY , zrotZ });
			// Lumière diffuse
			float dot_ap = a_dir.x * pxl_dir.x + a_dir.y * pxl_dir.y + a_dir.z * pxl_dir.z; // TODO : Tweaker selon la distance au soleil
			// Lumière réfléchie
			point3 v_reflect = unit3(sum3(a_dir, scale3(pxl_dir, 2), 1));
			float dot_cp = v_reflect.x * v_ac.x + v_reflect.y * v_ac.y + v_reflect.z * v_ac.z;

			float coef_diffuse = a->type == ETOILE ? 1 : sigmoid(10 * dot_ap) * 0.95f; // Sigmoide ? euuuh ca lag ca non ?
			float coef_reflect = a->type == ETOILE ? 0 : dot_cp;

			point3 pxl_col = unit3((point3) { ((Uint8*)pxl)[0], ((Uint8*)pxl)[1], ((Uint8*)pxl)[2] });

			//pxls[(int)x + (int)(y * w)] = scalePxl(pxl, coef_diffuse,  powf(coef_reflect * pxl_col.z, 7) * 255);

		}
	}

}

void renderAstre3(SDL_Surface* surface, cam* c, astre* a) {

	SDL_Surface* texture = a->texture;
	if (!texture) return;

	int w = surface->w, h = surface->h, w_texture = texture->w, h_texture = texture->h;
	Uint32* pxls = surface->pixels;
	Uint8* pxls_texture = texture->pixels, * pxl;


	point3 a_pos = a->position;
	point2 p_screen = projectPoint(c, a_pos, w, h);
	if (p_screen.x == -1 && p_screen.y == -1) return;

	float p_x = p_screen.x;
	float p_y = p_screen.y;

	point3 v_ac = sum3(c->pos, a_pos, -1);
	float dist = norm3(v_ac);
	float radius = a->radius * pow(10, a->radiusOrder);

	int screen_radius = radius * c->d_plan / (dist * c->window_ratio);

	if (screen_radius <= 0) return;

	// TODO : rotation selon l'axe astre caméra
	v_ac = scale3(v_ac, 1.f / dist);
	float lat, lat1, init_lat = -asinf(v_ac.z);// -a->latRotAxis;
	float lon, lon1, init_lon = atanf(v_ac.y / v_ac.x);

	float init_cos_lat = fast_cos(init_lat), init_sin_lat = fast_sin(init_lat), init_sin_lat1;

	if (v_ac.x > 0) init_lon = init_lon + PI;
	//init_lon *= (1.f + cosf(2.f * asinf(v_ac.z) + PI)) / 2.f;

	//printf("[ %.2f ; %.2f ] et [ %.2f ; %.2f ; %.2f ]\n", init_lon, init_lat, c->pos.x, c->pos.y, c->pos.z);

	point3 a_dir = unit3(a_pos);

	// Problèmes
	// - init_lon change brusquamment quand on passe au dessus du pôle
	// - translation latitudinale seulement sur l'axe Y
	// - faire tourner selon l'axe ac en fonction de la longitude de c
	// - Eclairage du soleil -> à faire selon coordonnées indépendantes

	// Rotation d'un point
	//		[ cos(ϕ) * cos(θ) ]   [ x ]
	// P =  [ cos(ϕ) * sin(θ) ] = [ y ]
	//		[	   sin(ϕ)	  ]   [ z ]
	// 
	// ϕ = asin(z) et θ = acos(x / cos(asin(z))
	// 
	//		[ cos(a)   0   sin(a) ] [ x ]
	// Ry = [   0      1     0    ] [ y ]
	//		[ sin(a)   0  -cos(a) ] [ z ]

	float X, Y, Z, den;
	int x_texture, y_texture;
	int x_start = p_x - screen_radius, x_end = p_x + screen_radius;
	int y_start = p_y - screen_radius, y_end = p_y + screen_radius;
	for (float y = y_start; y <= y_end; y++) {

		if (y <= 0 || y > h) continue;

		Y = (float)(y - p_y) / screen_radius;

		for (float x = x_start; x <= x_end; x++) {

			if (screen_radius < norm2((point2) { x - p_x, y - p_y })) continue;
			if (x <= 0 || x >= w) continue;

			X = (float)(x - p_x) / screen_radius;
			Z = sqrtf(1 - X * X - Y * Y);

			lat = fast_asin(Y);
			lon = X == 0 ? PI : fast_atan(Z / X) + PI_2;
			//lon += init_lon;

			init_sin_lat1 = init_sin_lat * (X < 0 ? -1 : 1);

			den = init_cos_lat * fast_cos(lat) * fast_cos(lon) + init_sin_lat1 * fast_sin(lat);

			// Rotations
			lon1 = fast_atan(fast_cos(lat) * fast_sin(lon) / den);
			if (den < 0 && X < 0 || den >= 0 && X >= 0) lon1 += PI;
			lat1 = fast_acos(init_sin_lat1 * fast_cos(lat) * fast_cos(lon) - init_cos_lat * fast_sin(lat));

			//pxl =  ((int)(lon1 * 180.f / PI) % 10 == 0 || (int)(lat1 * 180.f / PI) % 10 == 0) ? WHITE : 0;
			x_texture = w_texture - (lon1 - 0 * a->lonRotAxis) * w_texture / PI2;
			y_texture = lat1 * h_texture / PI;

			x_texture = (x_texture + w_texture) % w_texture;
			y_texture = (y_texture + h_texture) % h_texture;
			pxl = pxls_texture + (x_texture + y_texture * w_texture) * 3;

			//float r_sun = sqrtf((lon_sun - lon1) * (lon_sun - lon1) + lat1 * lat1);
			point3 pxl_dir = unit3((point3) { fast_sin(lat1)* fast_cos(lon1), fast_sin(lat1)* fast_sin(lon1), fast_cos(lat1) });
			//point3 pxl_dir = unit3((point3) { X, Z, Y });
			// Lumière diffuse
			float dot_ap = a_dir.x * pxl_dir.x + a_dir.y * pxl_dir.y + a_dir.z * pxl_dir.z; // TODO : Tweaker selon la distance au soleil
			// Lumière réfléchie
			point3 v_reflect = sum3(a_dir, scale3(pxl_dir, 2), 1);
			float dot_cp = v_reflect.x * v_ac.x + v_reflect.y * v_ac.y + v_reflect.z * v_ac.z;
			float coef_diffuse = a->type == ETOILE ? 1 : sigmoid(10 * dot_ap) * 0.95f;
			float coef_reflect = a->type == ETOILE ? 0 : dot_cp;

			pxls[(int)x + (int)(y * w)] = 0 ? 0xFF000000 | (int)(pxl_dir.x * 127 + 128) << 16 | (int)(pxl_dir.z * 127 + 128) << 8 | (int)(pxl_dir.y * 127 + 128) :
				pxl_dir.x > 0.99f ? 0xFFFF0000 : pxl_dir.y > 0.99f ? 0xFF0000FF : pxl_dir.z > 0.99f ? 0xFF00FF00 :
				scalePxl(pxl, 1 + 0 * coef_diffuse, 0 * coef_reflect * 128);
			//pxls[(int)x + (int)(y * w)] = (1 && (a->type == ETOILE  || r_sun > 0.)) ? 0xFF << 24 | pxl[0] << 16 | pxl[1] << 8 | pxl[2] : 0xFF000000;

		}
	}

}

void renderAstre2(SDL_Surface* surface, cam* c, astre* a) {

	float unitPerKm = 1.f / 1000;
	float pixelPerUnit = 5;

	int w = surface->w, h = surface->h;
	Uint32* pxls = surface->pixels;

	point2 p_screen = projectPoint(c, a->position, w, h);
	int p_x = p_screen.x;
	int p_y = p_screen.y;

	point3 v_ac = sum3(c->pos, a->position, -1);
	float dist = norm3(v_ac);
	float radius = a->radius * pow(10, a->radiusOrder);

	int screen_radius = radius * c->d_plan / (dist * c->window_ratio);

	if (screen_radius <= 0) return;

	Uint32 color = a->type == ETOILE ? WHITE : BLUE;


	v_ac = scale3(v_ac, 1.f / dist);
	float lat, init_lat = -asinf(v_ac.z) - a->latRotAxis;
	float lon, init_lon = atanf(v_ac.y / v_ac.x) - a->lonRotAxis;

	//init_lon = 0;
	//init_lat = - PI / 3;
	printf("[ %.2f ; %.2f ] et [ %.2f ; %.2f ; %.2f ]\n", init_lon, init_lat, c->pos.x, c->pos.y, c->pos.z);

	// Rotation d'un point
	//		[ cos(ϕ) * cos(θ) ]   [ x ]
	// P =  [ cos(ϕ) * sin(θ) ] = [ y ]
	//		[	   sin(ϕ)	  ]   [ z ]
	// 
	// ϕ = asin(z) et θ = acos(x / cos(asin(z))


	int x_start, x_end;
	int y_start = p_y - screen_radius, y_end = p_y + screen_radius, y2;
	for (int y = y_start; y <= y_end; y++) {

		y2 = 2 * p_y - y;
		if ((y <= 0 || y > h) && (y2 <= 0 || y2 > h)) continue;

		float r_x = sinf(acosf((float)(screen_radius - y + y_start) / screen_radius)) * screen_radius;
		x_start = p_x - r_x;
		x_end = p_x + r_x;


		for (int x = x_start; x <= x_end; x++) {
			if (x <= 0 || x >= w) continue;

			float X = (float)(x - p_x) / screen_radius;
			float Y = (float)(y - p_y) / screen_radius;
			float Z = sqrtf(1 - X * X - Y * Y);

			lat = asinf(Y);

			lon = atanf(Z / X) + PI / 2;
			lon += init_lon;

			float init_lat1 = init_lat * (X < 0 ? -1 : 1);

			float lon1 = atanf(cosf(lat) * sinf(lon) / (cosf(init_lat1) * cosf(lat) * cosf(lon) + sinf(init_lat1) * sinf(lat))) - a->lonRotAxis;
			float lat1 = acosf(sinf(init_lat1) * cosf(lat) * cosf(lon) - cosf(init_lat1) * sinf(lat));

			color = (((int)(lon1 * 180.f / PI) % 10) == 0 || ((int)(lat1 * 180.f / PI) % 10) == 0) ? WHITE : 0;

			if (y >= 0 && y < h) pxls[x + y * w] = color;
			//if (y2 >= 0 && y2 < h) pxls[x + y2 * w] = color;

		}
	}

}

// Graphics
void mapBackground(SDL_Surface* surface, SDL_Surface* texture, cam* c, float ratio) {// ratio : surface / texture

	Uint32* pxls_surface = surface->pixels;
	Uint8* pxls_texture = texture->pixels;

	float c_lon = c->lon, c_lat = c->lat;
	int w_surface = surface->w, h_surface = surface->h;
	int w_texture = texture->w, h_texture = texture->h;
	int x_texture, y_texture;

	float ratio_w = (float)w_surface / w_texture;
	float ratio_h = (float)h_surface / h_texture;


	// d_lon et d_lat en rad / pxl
	float d_lon_texture = PI2 / w_texture, d_lat_texture = PI / h_texture;
	float d_lon_surface = (d_lon_texture * ratio) / ratio_w, d_lat_surface = (d_lat_texture * ratio) / ratio_h;

	// Ecran déformé autour du point lon=c_lon ; lat=0, puis translation vers point cam de lat radiants :
	// si lon du point = c_lon		: lon+=0 et lat+=c_lat
	// si lon du point = c_lon+Pi/2	: lon+=0 et lat+=0
	// si lon entre les deux		: lon+=sin(2 * (lon - c_lon))*facteur ? etlat+=cos(lon - c_lon) * c_lat

	// On veut le bord supérieur de l'écran, on part de la latitude caméra
	// On y ajoute la demi-hauteur de l'écran que l'on ramène au nombre de pixels pour la texture en divisant par ratio
	// Puis on mutliplie par d_lat_texture pour avoir la latitude à ajouter pour atteindre le bord.
	// Idem pour la longitude
	float lat = c_lat + d_lat_surface * h_surface / 2.f;
	float lon = c_lon - d_lon_surface * w_surface / 2.f;
	float lon0 = lon, lat0 = lat;

	for (int y = 0; y < h_surface; y++) {

		if (lat >= PI_2 || lat <= -PI_2) {
			lon0 += PI * (lon >= PI ? -1 : 1);
			lat = (lat >= PI_2 ? PI : -PI) - lat;
			d_lat_surface *= -1;
		}

		lon = lon0;

		for (int x = 0; x < w_surface; x++) {

			if (lon >= PI2)		lon -= PI2;
			else if (lon < 0)	lon += PI2;

			x_texture = (int)(w_texture * lon / PI) % w_texture;
			y_texture = (1 - lat / PI_2) * h_texture / 2.f;

			Uint8* pxl = pxls_texture + (x_texture + y_texture * w_texture) * 3;

			*(pxls_surface + x + y * w_surface) = 0xFF << 24 | pxl[0] << 16 | pxl[1] << 8 | pxl[2];

			/*if (x == w_surface - 1 || x == 0 || y == h_surface - 1 || y == 0)
				*(pxls_surface + (int)(x_texture * ratio_w) + (int)(y_texture * ratio_h) * w_surface) = GREEN;
*/

			lon += d_lon_surface;
		}

		lat -= d_lat_surface;

	}
}

void mapBackground2(SDL_Surface* surface, SDL_Surface* texture, cam* c, float ratio) {// ratio : surface / texture

	Uint32* pxls_surface = surface->pixels;
	Uint8* pxls_texture = texture->pixels;

	float c_lon = c->lon, c_lat = c->lat;
	int w_surface = surface->w, h_surface = surface->h;
	int w_texture = texture->w, h_texture = texture->h;
	int x_texture, y_texture;

	float ratio_w = (float)w_surface / w_texture;
	float ratio_h = (float)h_surface / h_texture;


	// d_lon et d_lat en rad / pxl
	float d_lon_texture = PI2 / w_texture, d_lat_texture = PI / h_texture;
	float d_lon_surface = (d_lon_texture * ratio) / ratio_w, d_lat_surface = (d_lat_texture * ratio) / ratio_h;

	// Ecran déformé autour du point lon=c_lon ; lat=0, puis translation vers point cam de lat radiants :
	// si lon du point = c_lon		: lon+=0 et lat+=c_lat
	// si lon du point = c_lon+Pi/2	: lon+=0 et lat+=0
	// si lon entre les deux		: lon+=sin(2 * (lon - c_lon))*facteur ? et lat+=cos(lon - c_lon) * c_lat

	// On veut le bord supérieur de l'écran, on part de la latitude caméra
	// On y ajoute la demi-hauteur de l'écran que l'on ramène au nombre de pixels pour la texture en divisant par ratio
	// Puis on mutliplie par d_lat_texture pour avoir la latitude à ajouter pour atteindre le bord.
	// Idem pour la longitude
	float lat = c_lat + d_lat_surface * h_surface / 2.f;
	float lon = c_lon - d_lon_surface * w_surface / 2.f;
	float lon0 = lon, lat0 = lat;

	for (int y = 0; y < h_surface; y++) {

		//if (lat >= PI_2 || lat <= -PI_2) {
		//	lon0 += PI * (lon0 >= PI ? -1 : 1);
		//	lat = (lat >= PI_2 ? PI : -PI) - lat;
		//	d_lat_surface *= -1;
		//}

		if (y == h_surface / 2) {
			float i = 0;
		}
		if (y == h_surface - 1) {
			float k = 0;
		}

		lon = lon0;

		for (int x = 0; x < w_surface; x++) {

			if (x == w_surface / 2) {
				float j = 0;
			}
			if (x == w_surface - 1) {
				float l = 0;
			}

			if (lon >= PI2)		lon -= PI2;
			else if (lon < 0)	lon += PI2;

			float a = w_texture / 2;

			//x_texture = (int)(w_texture * lon / PI2) % w_texture;
			//y_texture = (1 - lat / PI_2) * h_texture / 2.f;
			float lon1 = lon - c_lon;
			float lat1 = lat < 0 ? -PI + 2 * lat * sinf(lat) : PI - 2 * lat * sinf(lat);
			
			x_texture = a * (c_lon / PI + sinhf(lon1) / (coshf(lat1) - cosf(lon1)));
			y_texture = a * sinf(lat1) / (coshf(lat1) - cosf(lon1)) + h_texture / 2;

			if (y_texture >= h_texture || y_texture < 0) {
				x_texture += w_texture / 2 * (x_texture >= w_texture/2 ? -1 : 1);
				y_texture = (y_texture >= h_texture ? h_texture : -h_texture) - y_texture;
				//d_lat_surface *= -1;
			}


			Uint8* pxl = pxls_texture + (x_texture + y_texture * w_texture) * 3;

			if (x_texture >= 0 && x_texture < w_texture && y_texture > 0 && y_texture < h_texture) {
				*(pxls_surface + x + y * w_surface) = 0xFF << 24 | pxl[0] << 16 | pxl[1] << 8 | pxl[2];

				if (x == w_surface - 1 || x == 0 || y == h_surface - 1 || y == 0)
					*(pxls_surface + (int)(x_texture * ratio_w) + (int)(y_texture * ratio_h) * w_surface) = GREEN;
			}
			


			lon += d_lon_surface;
		}

		lat -= d_lat_surface;

	}
}

void mapPxls(SDL_Surface* destination, SDL_Surface* source) {
	int w_dst = destination->w, h_dst = destination->h;
	int w_src = source->w, h_src = source->h;
	int pitch = source->pitch;
	Uint32* pxls = destination->pixels;
	Uint8* img_pxls = source->pixels;

	for (int x = 0; x < w_dst; x++)
		if (x < w_src)
			for (int y = 0; y < h_dst; y++)
				 if (y < h_src)
					*(pxls + x + y * w_dst) = getPxl32(img_pxls + (x + y * w_src) * 3);

}
Uint32 getPxl32(Uint8* pxl) {
	return 0xFF << 24 | pxl[0] << 16 | pxl[1] << 8 | pxl[2];
}


void drawLine(SDL_Surface* s, point2 p1, point2 p2, Uint32 color) {

	int w = s->w, h = s->h;
	point2 v = sum2(p2, p1, -1);
	float n = min(norm2(v), sqrtf(w*w + h*h));
	v = scale2(v, 1.f / n);

	Uint32* pxls = s->pixels;

	int x, y;
	for (int k = 0; k < n; k++) {
		x = p1.x + v.x * k;
		if (x < 0 || x >= w) continue;
		y = p1.y + v.y * k;
		if (y < 0 || y >= h) continue;

		*(pxls + x + y * w) = color;
	}

}

// Utils
point3* createPoint(float x, float y, float z) {
	point3* p = (point3*)calloc(1, sizeof(point3));
	p->x = x;
	p->y = y;
	p->z = z;

	return p;
}
point3 sum3(point3 p1, point3 p2, int diff) {
	if (diff != -1) diff = 1; 
	p1.x += p2.x * diff;
	p1.y += p2.y * diff;
	p1.z += p2.z * diff;
	return p1;
}
point2 sum2(point2 p1, point2 p2, int diff) {
	if (diff != -1) diff = 1;
	p1.x += p2.x * diff;
	p1.y += p2.y * diff;
	return p1;
}
point3 scale3(point3 p, float f) {
	p.x *= f;
	p.y *= f;
	p.z *= f;
	return p;
}
point2 scale2(point2 p, float f) {
	p.x *= f;
	p.y *= f;
	return p;
}
float norm3(point3 p) {
	return sqrtf(p.x * p.x + p.y * p.y + p.z * p.z);
}
float norm2(point2 p) {
	return sqrtf(p.x * p.x + p.y * p.y);
}
point3 unit3(point3 p) {
	return scale3(p, 1.f / norm3(p));
}
point2 unit2(point2 p) {
	return scale2(p, 1.f / norm2(p));
}
point3 polaireToCartesien(float lat, float lon) {
	return (point3) {
		cosf(lat) * cosf(lon),
		cosf(lat) * sinf(lon),
		sinf(lat)
	};
}

Uint32 scalePxl(Uint8* pxl, float coef, float val) {
	int r = pxl[0] * coef + val; r = r > 255 ? 255 : (r < 0 ? 0 : r);
	int g = pxl[1] * coef + val; g = g > 255 ? 255 : (g < 0 ? 0 : g);
	int b = pxl[2] * coef + val; b = b > 255 ? 255 : (b < 0 ? 0 : b);

	return 0xFF << 24 | r << 16 | g << 8 | b;
}
float sigmoid(float x) {
	return 1.f / (1.f + pow(E, -x));
}

// Optimistaion Trigo
void init_TUL() {
	for (float i = 0; i < TUL_SIZE; i++) {
		asin_table[(int)i] = asinf(i / TUL_SIZE);
		atan_table[(int)i] = atanf(15 * i / TUL_SIZE);
		sin_table[(int)i] = sinf(PI2 * i / TUL_SIZE);
	}
}
float fast_cos(float x) {
	//return fast_sin(x + PI_2);
	return fast_sin(x + PI_2);
}
float fast_sin(float x) {
	//return (x - x * fabs(x) / PI) * 4.f / PI;
	return sin_table[(int)(fabs(x) * TUL_SIZE/PI2) % TUL_SIZE] * (x < 0 ? -1 : 1);
}
float fast_acos(float x) {
	return PI_2 - fast_asin(x);
	//return acos_table[(int)((x + 1) * TUL_SIZE / 2)];
}
float fast_asin(float x) {
	//float x2 = x * x;
	//return x * (1.0f + x2 * (0.16666667f + x2 * 0.075f));
	return asin_table[(int)(fabs(x) * TUL_SIZE)] * (x < 0 ? -1 : 1);
}
float fast_atan(float x) {
	return atanf(x);
	return (fabs(x) < 15 ? atan_table[(int)(fabs(x) * TUL_SIZE / 15)] : PI_2) * (x < 0 ? -1 : 1);
}

// FREE
void freeList(list* l) {
	if (!l) return;
	if (l->next) freeList(l->next);
	freeAstre(l->p_astre);
	free(l);
}

void freeAstre(astre* a) {
	SDL_FreeSurface(a->texture);
	free(a);
}