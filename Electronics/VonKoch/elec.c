#include "elec.h"

point* createPoint(list* l, point p) {
	point* pp = (point*)calloc(1, sizeof(point));
	pp->pnext_Connect = NULL;
	pp->pprec_Connect = NULL;
	pp->x = p.x;
	pp->y = p.y;

	tryConnect(l, pp);

	return pp;
}

elec* createElec(list* l, point p1, point p2, type t) {
	if (p1.x == p2.x && p1.y == p2.y) return NULL;

	elec* e = (elec*)calloc(1, sizeof(elec));
	e->p1 = createPoint(l, p1);
	e->p2 = createPoint(l, p2);
	e->t = t;
	e->selected = 1;
	return e;
}

list* createList(elec* e) {
	list* l = (list*)calloc(1, sizeof(list));
	l->e = e;
	l->next = NULL;
	l->prec = NULL;
	return l;
}

void addList(list** l, elec* e) {
	if (!e) return;
	if (!l) return;
	if (!*l) {
		*l = createList(e);
		printList(*l);
		return;
	}
	list* tmp = createList(e);
	tmp->next = *l;
	(*l)->prec = tmp;
	*l = tmp;
	printList(*l);
}

void deleteSelected(list** l) {
	
	if (!*l || !l) return;
	list* tmp = NULL;
	elec* e = (*l)->e;
	if (e->selected) {
		disconnect(e->p1);
		disconnect(e->p2);
		free(e);
		tmp = *l;
		if (tmp->next) (*l)->next->prec = (*l)->prec;
		if (tmp->prec) {
			deleteSelected(&(*l)->next);
			(*l)->prec->next = (*l)->next;
		}
		else {
			*l = (*l)->next;
			deleteSelected(l);
		}
	}
	else deleteSelected(&(*l)->next);
	if (tmp) free(tmp);
}

// On insère le maillon p dans la chaine pv
void connect(point* p, point* pv) {
	// Connection dans les 2 sens à l'avant.
	if (pv->pnext_Connect) {
		p->pnext_Connect = pv->pnext_Connect;	// pv lui passe ses maillons suivants ...
		pv->pnext_Connect->pprec_Connect = p;	// et le maillon suivant prend p comme précédent
	}
	
	// Connection dans les 2 sens à l'arrière.
	pv->pnext_Connect = p;						// p devient le maillon suivant de pv.
	p->pprec_Connect = pv;						// pv devient le maillon precedant de p.
}

void disconnect(point* p) {
	if (p->pnext_Connect) p->pnext_Connect->pprec_Connect = p->pprec_Connect;
	if (p->pprec_Connect) p->pprec_Connect->pnext_Connect = p->pnext_Connect;

	p->pnext_Connect = NULL;
	p->pprec_Connect = NULL;
}

void tryConnect(list* l, point* p) {
	point* p1, * p2;
	while (l) {
		p1 = l->e->p1;
		p2 = l->e->p2;

		if (p->x == p1->x && p->y == p1->y && p != p1 && !areConnected(p, p1)) {
			connect(p, p1);
			break;
		}
		if (p->x == p2->x && p->y == p2->y && p != p2 && !areConnected(p, p2)) {
			connect(p, p2);
			break;
		}

		l = l->next;
	}
}

int areConnected(point* p, point* pv) {
	point* pTmp = p->pnext_Connect;
	while (pTmp) {
		if (pv == pTmp) return 1;
		pTmp = pTmp->pnext_Connect;
	}
	pTmp = p->pprec_Connect;
	while (pTmp) {
		if (pv == pTmp) return 1;
		pTmp = pTmp->pprec_Connect;
	}
	return 0;
}

void printList(list* l) {

	const char* enumToString[] = { "GENERATEUR", "RESISTANCE", "BOBINE", "CONDENSATEUR", "DIODE", "WIRE" };

	point* p1, *p2;
	while (l) {
		p1 = l->e->p1;
		p2 = l->e->p2;
		printf("[%.0f,%.0f|%.0f,%.0f|%s]->", p1->x, p1->y, p2->x, p2->y, enumToString[l->e->t]);
		l = l->next;
	}
	printf("\n");
}

void freeList(list* l) {
	if (!l) return;
	freeList(l->next);
	free(l->e->p1);
	free(l->e->p2);
	free(l->e);
	free(l);
}