#include <stdio.h>
#include <stdlib.h>
#include "mot.h"
#include "utils_SDL.h"

bloc* createBloc(point a, point b, stat s) {

	bloc* rect = (bloc*)calloc(1, sizeof(bloc));

	rect->a.m_x = a.m_x;
	rect->a.m_y = a.m_y;
	rect->b.m_x = b.m_x;
	rect->b.m_y = b.m_y;
	rect->s = s;

	return rect;

}

list* createList(bloc* rect) {

	list* l = (list*)calloc(1, sizeof(1));

	l->rect = rect;

	return l;

}

void insertList(list** l, bloc* rect) {

	if (!&l || !l) {
		*l = createList(rect);
		return;
	}

	list* head = createList(rect);
	head->next = *l;
	*l = head;

}

list* popList(list** l) {

	if (!&l || !l || !*l) return NULL;

	list* tmp = *l;
	*l = (*l)->next;

	return tmp;

}

void printList(list* l) {

	if (!l) {
		printf("X\n\n");
		return;
	}

	printf("[(%d,%d)>(%d,%d) s:%d]->", l->rect->a.m_x, l->rect->a.m_y, l->rect->b.m_x, l->rect->b.m_y, l->rect->s);
	printList(l->next);

}

void affBloc(bloc* rect, SDL_Surface * window) {

	point a = rect->a;
	point b = rect->b;

	_SDL_DrawLine(window, a.m_x, a.m_y, a.m_x, b.m_y, 255 * (rect->s == DYNAMIC), 255 * (rect->s == STATIC), 100);
	_SDL_DrawLine(window, a.m_x, b.m_y, b.m_x, b.m_y, 255 * (rect->s == DYNAMIC), 255 * (rect->s == STATIC), 100);
	_SDL_DrawLine(window, b.m_x, b.m_y, b.m_x, a.m_y, 255 * (rect->s == DYNAMIC), 255 * (rect->s == STATIC), 100);
	_SDL_DrawLine(window, b.m_x, a.m_y, a.m_x, a.m_y, 255 * (rect->s == DYNAMIC), 255 * (rect->s == STATIC), 100);

}

void affAll(list* l, SDL_Surface* window) {

	list* tmp = l;

	while (tmp) {
		affBloc(tmp->rect, window);
		tmp = tmp->next;
	}

}

void moveBloc(list* l, dep d, float* dx, float* dy) {

	list** tmp = isDynamic(l);

	if (!&tmp || !tmp || !*tmp) return;

	bloc* tmp2 = (*tmp)->rect;

	if (!canMoveBloc(l, tmp2, d, dx, dy)) return;
	

	switch (d)
	{
	case LEFT:
	case RIGHT:
		tmp2->a.m_x+=*dx;
		tmp2->b.m_x+=*dx;
		break;
	case UP:
	case DOWN:
		tmp2->a.m_y += *dy;
		tmp2->b.m_y += *dy;
		break;
	default:
		break;
	}

}

int canMoveBloc(list* l, bloc* rect, dep d, float *dx, float *dy) {

	list* tmp = l;

	switch (d)
	{
	case LEFT:
		while (tmp) {
			if (tmp->rect != rect && tmp->rect->b.m_x + 1 > rect->a.m_x + *dx && tmp->rect->a.m_x < rect->b.m_x && !(tmp->rect->b.m_y <= rect->a.m_y || tmp->rect->a.m_y >= rect->b.m_y)) {
				rect->b.m_x -= rect->a.m_x - tmp->rect->b.m_x - 1;
				rect->a.m_x = tmp->rect->b.m_x + 1;
				if (tmp->rect->s == DYNAMIC) {
					tmp->rect->a.m_x--;
					tmp->rect->b.m_x--;
					*dx += 0.1;
					return 0;
				}
				*dx *= -0.2;
				return 0;
			}
			tmp = tmp->next;
		}
		break;
	case RIGHT:
		while (tmp) {
			if (tmp->rect != rect && tmp->rect->a.m_x - 1 < rect->b.m_x + *dx && tmp->rect->b.m_x > rect->a.m_x && !(tmp->rect->b.m_y <= rect->a.m_y || tmp->rect->a.m_y >= rect->b.m_y)) {
				rect->a.m_x -= rect->b.m_x - tmp->rect->a.m_x + 1;
				rect->b.m_x = tmp->rect->a.m_x - 1;
				if (tmp->rect->s == DYNAMIC) {
					tmp->rect->a.m_x++;
					tmp->rect->b.m_x++;
					*dx-=0.1;
					return 0;
				}
				*dx *= -0.2;
				return 0;
			}
			tmp = tmp->next;
		}
		break;
	case UP:
		while (tmp) {
			if (tmp->rect != rect && tmp->rect->b.m_y + 1 > rect->a.m_y + *dy && tmp->rect->a.m_y < rect->b.m_y && !(tmp->rect->b.m_y <= rect->a.m_y || tmp->rect->a.m_y >= rect->b.m_y)) {
				rect->b.m_y -= rect->a.m_y - tmp->rect->b.m_y - 1;
				rect->a.m_y = tmp->rect->b.m_y + 1;
				*dy *= -0.2;;
				return 0;
			}
			tmp = tmp->next;
		}
		break;
	case DOWN:
		while (tmp) {
			if (tmp->rect != rect && tmp->rect->a.m_y - 1 < rect->b.m_y + *dy && tmp->rect->b.m_y > rect->a.m_y && !(tmp->rect->b.m_x <= rect->a.m_x || tmp->rect->a.m_x >= rect->b.m_x)){
				if (*dy > 0) {
					rect->a.m_y -= rect->b.m_y - tmp->rect->a.m_y + 1;
					rect->b.m_y = tmp->rect->a.m_y - 1;
				}
				*dy = 0;
				return 0;
			}
			tmp = tmp->next;
		}
		break;
	default:
		break;
	}

	//*dy += 0.981;

	return 1;
}

list** isDynamic(list* l) {

	list* tmp = l;

	while (tmp && tmp->rect->s == STATIC) tmp = tmp->next;

	return &tmp;

}

void freeList(list* l) {

	if (!l) return;

	freeList(l->next);

	free(l->rect);
	//free(l);

	printf("J'ai free !\n");

}