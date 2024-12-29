#include "elec.h"

point* createPoint(point p) { // TODO SUPPR DEPENDANCE A L
	point* pp = (point*)calloc(1, sizeof(point));
	pp->pnext_Connect = NULL;
	pp->pprec_Connect = NULL;
	pp->x = p.x;
	pp->y = p.y;
	pp->alt = 0;
	pp->V = 0;

	return pp;
}
elec* createElec(point p1, point p2, type t) {
	if (p1.x == p2.x && p1.y == p2.y) return NULL;

	elec* e = (elec*)calloc(1, sizeof(elec));
	e->p1 = createPoint(p1);
	e->p2 = createPoint(p2);
	e->p1->e = e;
	e->p2->e = e;
	e->t = t;
	e->selected = 1;
	if (t == GENERATEUR) e->U = 12;
	if (t == RESISTANCE) e->R = 1000;

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
		tryConnect(*l, e->p1);
		tryConnect(*l, e->p2);
		return;
	}
	tryConnect(*l, e->p1);
	tryConnect(*l, e->p2);
	list* tmp = createList(e);
	tmp->next = *l;
	(*l)->prec = tmp;
	*l = tmp;
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

	const char* enumToString[] = { "VCC", "GND", "GENERATEUR", "RESISTANCE", "BOBINE", "CONDENSATEUR", "DIODE", "WIRE"};

	point* p1, *p2;
	while (l) {
		p1 = l->e->p1;
		p2 = l->e->p2;
		printf("[%.0f,%.0f|%.0f,%.0f|%s]->", p1->x, p1->y, p2->x, p2->y, enumToString[l->e->t]);
		l = l->next;
	}
	printf("\n");
}
void printElec(elec* e) {
	const char* enumToString[8] = { "VCC", "GND", "GENERATEUR", "RESISTANCE", "BOBINE", "CONDENSATEUR", "DIODE", "WIRE" };
	printf("[%s | U : %.1fV (p1:%.1fV,p2:%.1fV) | I : %.1fmA | R : %fO] %d %d\n", enumToString[e->t], e->U, e->p1->V, e->p2->V, e->I * 1000, e->R, e->p1->alt, e->p2->alt);
}
void freeList(list* l) {
	if (!l) return;
	freeList(l->next);
	free(l->e->p1);
	free(l->e->p2);
	free(l->e);
	free(l);
	l = NULL;
}

list* simulate(list* l) {

	int nbElec = cntElecList(l), nbElecPrec;
	do { // Tant que ca a toujours un effet sur le nombre d'Elec...
		nbElecPrec = nbElec;
		// ...On Réduit en un circuit équivalent.
		l = makeSerialEqCirc(l, 1);
		l = makeDerivEqCirc(l, 2);
		nbElec = cntElecList(l);
	} while (nbElec != nbElecPrec);
	initV(l, 3);
	initI(l, 4);
	return l;
}

// TODO : OPTI les while avec des listes préfaites selon le type
void initV(list* l, int alt) {
	elec* e;

	// INIT VCC / GND
	list* ltmp = l;
	while (ltmp) {
		e = ltmp->e;
		if (e->t > GND) {
			ltmp = ltmp->next;
			continue;
		}

		e->p1->V = e->t == VCC ? 5 : 0;

		if (propagateV(e->p1, alt) == 0) printf("COURT CIRCUIT !\n");
		
		ltmp = ltmp->next;
	}

	// INIT GENERATEUR
	ltmp = l;
	point* p1, * p2;
	while (ltmp) {
		e = ltmp->e;
		if (e->t != GENERATEUR) {
			ltmp = ltmp->next;
			continue;
		}
		p1 = e->p1; p2 = e->p2;

		if (p1->alt == alt && p2->alt == alt && p2->V - p1->V != e->U) printf("GENERATEUR FORCE !\n");
		else if (p1->alt == alt) {
			p2->V = p1->V + e->U;
			if (propagateV(p2, alt) == 0) printf("COURT CIRCUIT G2 !\n");
		} else if (p2->alt == alt) {
			p1->V = p2->V - e->U;
			if (propagateV(p1, alt) == 0) printf("COURT CIRCUIT G1 !\n");
		} else {
			p1->V = -e->U / 2;
			p2->V =  e->U / 2;
			if (propagateV(p1, alt) == 0) printf("COURT CIRCUIT G12 !\n");
			if (propagateV(p2, alt) == 0) printf("COURT CIRCUIT G21 !\n");
		}

		ltmp = ltmp->next;
	}

}

void initI(list* l, int alt) {
	list* ltmp = l; elec* e; point* p1, * p2;
	while (ltmp) {
		e = ltmp->e;
		// On initialise ce pour quoi on peut calculer I.
		if (e->t > GENERATEUR && e->t != WIRE) {
			p1 = e->p1; p2 = e->p2;
			if (p1->alt != alt && p2->alt != alt) {
				e->U = fabs(p2->V - p1->V);
				e->I = e->U / e->R;

				// Propagation de I en série
				if (!isBifurc(p1)) propagateI(neighborSwitch(p1), e->I, alt);
				else nodeSum(p1, alt);
				if (!isBifurc(p2)) propagateI(neighborSwitch(p2), e->I, alt);
				else nodeSum(p2, alt);

				p1->alt = p2->alt = alt;

			}

		}
		ltmp = ltmp->next;
	}

	/*ltmp = l;
	while (ltmp) {
		e = ltmp->e;
		// Initialise le reste.
		if (e->t == GENERATEUR || e->t == WIRE) {
			p1 = e->p1; p2 = e->p2;
			if (p1->alt != alt && isBifurc(p1)) nodeSum(p1, alt);
			if (p2->alt != alt && isBifurc(p2)) nodeSum(p2, alt);
			ltmp = ltmp->next;
		}
	}*/
}
int propagateV(point* p, int alt) {
	if (p->alt == alt) return 1;
	p->alt = alt;

	int tmp = propagateForward(p, alt);
	return propagateBackward(p, alt) && tmp;
}
void propagateI(point* p, float I, int alt) {
	if (!p) return;
	p->alt = alt;
	I = p->e->t == GENERATEUR ? -I : I;
	p->e->I = I;
	p = poleSwitch(p);
	if (!isBifurc(p)) propagateI(neighborSwitch(p), I, alt);
	else nodeSum(p, alt);
}
// Loi des mailles
void nodeSum(point* p, int alt) {
	p->alt = alt;
	float Isum = enterOrExitNode(p);
	point* ptmp = p->pnext_Connect, * undefinedI = NULL;
	// On parcours les pôles voisins d'une maille
	while (ptmp) {
		if (ptmp->alt != alt) { // Si un n'est pas défini...
			if (undefinedI) return; // Si déjà un n'est pas défini, on sort (2 indéfinis = 2 inconnues dans 1 équation).
			undefinedI = ptmp; // On le sauvegarde.
		}
		else Isum += enterOrExitNode(ptmp); // Si défini on ajoute à la somme des I en prenant en compte si le courant est entrant ou sortant.
		ptmp = ptmp->pnext_Connect;
	}
	ptmp = p->pprec_Connect;
	while (ptmp) {
		if (ptmp->alt != alt) {
			if (undefinedI) return;
			undefinedI = ptmp;
		}
		else Isum += enterOrExitNode(ptmp);
		ptmp = ptmp->pprec_Connect;
	}
	if (!undefinedI) {
		printf("Isum = %.1f\n", Isum); // Doit être nul.
		return; // Si tout le monde est défini, on sort.
	}

	propagateI(undefinedI, -Isum, alt);

}
float enterOrExitNode(point* p) {
	return p->e->I * (p->V > poleSwitch(p)->V ? -1 : 1);
}

int propagateForward(point* p, int alt) {

	float V = p->V;
	elec* e = p->e, *e_;

	point* pTmp = p->pnext_Connect, *pProp;
	type t;
	int ret = 1;
	while (pTmp) {
		if (pTmp->alt == alt) return 0;
		pTmp->alt = alt;

		e_ = pTmp->e;
		t = e_->t;
		if (t == GND) return V == 0;
		if (t == VCC) return V != 0;
		pTmp->V = V;
		if (t == WIRE) {
			pProp = poleSwitch(pTmp);
			pProp->V = V;
			ret = propagateV(pProp, alt) && ret;
		}
		pTmp = pTmp->pnext_Connect;
	}

	return ret;
}
int propagateBackward(point* p, int alt) {

	float V = p->V;
	elec* e = p->e, * e_;

	point* pTmp = p->pprec_Connect, * pProp;
	type t;
	int ret = 1;
	while (pTmp) {
		if (pTmp->alt == alt) return 0;
		pTmp->alt = alt;

		e_ = pTmp->e;
		t = e_->t;
		if (t == GND) return V == 0;
		if (t == VCC) return V != 0;
		pTmp->V = V;
		if (t == WIRE) {
			pProp = poleSwitch(pTmp);
			pProp->V = V;
			ret = propagateV(pProp, alt) && ret;
		}
		else pTmp->alt = alt;
		pTmp = pTmp->pprec_Connect;
	}

	return ret;
}

list* makeDerivEqCirc(list* l, int alt) {
	elec* e, *e_;
	point* ptmp, ptmp2;
	float REq;
	list* lEq = NULL, *ltmp = l;
	while (ltmp) {
		e = ltmp->e;
		if (e->t > GENERATEUR && (e->p1->alt == alt || e->p2->alt == alt)) {
			ltmp = ltmp->next;
			continue;
		}
		e->p1->alt = e->p2->alt = alt;
		
		if (e->t == RESISTANCE) {
			REq = 1.f / e->R;

			REq += checkDeriv(e->p1->pnext_Connect, e, alt, pnext);
			REq += checkDeriv(e->p1->pprec_Connect, e, alt, pprec);

			e_ = createElec(*e->p1, *e->p2, 1.f / REq == 0 ? WIRE : RESISTANCE);
			e_->R = 1.f / REq;
			addList(&lEq, e_);
		} else addList(&lEq, createElec(*e->p1, *e->p2, e->t));

		ltmp = ltmp->next;
	}

	freeList(l);
	return lEq;

}

float checkDeriv(point* p, elec* e, int alt, point* (*dir)(point*)) {
	elec* e_;
	float REq = 0;
	while (p) {
		if (p->e->t > GENERATEUR &&  p->alt != alt && areConnected(poleSwitch(p), e->p2)) {
			e_ = p->e;
			REq += 1.f / e_->R;
			e_->p1->alt = e_->p2->alt = alt;
		}
		p = dir(p);
	}
	return REq;
}

list* makeSerialEqCirc(list* l, int alt) {
	elec* e;
	list* lEq = NULL, *ltmp = l;
	point* ptmp, *p;
	while (ltmp) {
		e = ltmp->e;
		p = e->p1;

		if (e->t <= GENERATEUR) {						// On part du VCC/GND/GEN où les potentiels sont déjà définis
			addList(&lEq, createElec(*p, *e->p2, e->t));// Ajout du VCC/GND/GEN dans la liste Equivalente

			// Ajout dans la liste des RESISTANCE Equivalente
			if (isBifurc(p)) {
				ptmp = p->pnext_Connect;
				while (ptmp) { fuseLists(&lEq, makeSerialREq(ptmp, alt)); ptmp = ptmp->pnext_Connect; }
				ptmp = p->pprec_Connect;
				while (ptmp) { fuseLists(&lEq, makeSerialREq(ptmp, alt)); ptmp = ptmp->pprec_Connect; }
			}
			else fuseLists(&lEq, makeSerialREq(neighborSwitch(p), alt));
		}

		ltmp = ltmp->next;
	}

	free(l);
	return lEq;

}

// Circuit avec REq = somme des R en série (maille sans bifurcations)
list* makeSerialREq(point* p, int alt) {

	list* lEq = NULL;

	int adone = 0;
	float REq = 0;
	point* ptmp = p;
	elec* e = p->e;
	while (1) {
		if (adone = (ptmp->alt == alt)) break;
		ptmp->alt = alt;
		
		if (e->t <= GENERATEUR) break;					// Si VCC/GND/GEN -> Série finie
		REq += e->R;									// Ajout de R
		ptmp = poleSwitch(ptmp); ptmp->alt = alt;		// Rebasement sur l'autre Pole de l'Elec
		if (isBifurc(ptmp)) break;						// Si bifurc -> Série finie
		ptmp = neighborSwitch(ptmp); e = ptmp->e;		// Changement d'Elec
	}
	if (p != ptmp) {
		// Création et ajout dans la liste de la RESISTANCE Equivalente
		elec* eEq = createElec(*p, *ptmp, REq == 0 ? WIRE : RESISTANCE);
		eEq->R = REq;
		addList(&lEq, eEq);
	}

	if (ptmp->e->t <= GENERATEUR || adone) return lEq;	// Si VCC/GND/GEN -> on va pas plus loin

	// Parcours des voisins du point
	point* ptmp2 = ptmp->pnext_Connect;
	while (ptmp2) { fuseLists(&lEq, makeSerialREq(ptmp2, alt)); ptmp2 = ptmp2->pnext_Connect; }
	ptmp2 = ptmp->pprec_Connect;
	while (ptmp2) { fuseLists(&lEq, makeSerialREq(ptmp2, alt)); ptmp2 = ptmp2->pprec_Connect; }

	return lEq;
}

// Renvoie 1 si le point est à une bifurcation (nb voisins > 1)
int isBifurc(point* p) {
	int found = 0;

	point* ptmp = p->pnext_Connect;
	while (ptmp) {
		found++;
		if (found > 1) return 1;
		ptmp = ptmp->pnext_Connect;
	}

	ptmp = p->pprec_Connect;
	while (ptmp) {
		found++;
		if (found > 1) return 1;
		ptmp = ptmp->pprec_Connect;
	}
	return 0;
}

// Fusionne 2 listes dans la première en libérant la 2eme liste (sans vérification de duplication d'elec)
void fuseLists(list** l1, list* l2) {
	if (!l2 || !*l1) return;
	list* ltmp;
	while (l2) {
		addList(l1, l2->e);
		ltmp = l2;
		l2 = l2->next;
		free(ltmp);
	}
	return *l1;
}

point* neighborSwitch(point* p) {
	return p->pnext_Connect ? p->pnext_Connect : p->pprec_Connect;
}
point* poleSwitch(point* p) {
	elec* e = p->e;
	return p == e->p1 ? e->p2 : e->p1;
}

int cntElecList(list* l) {
	int nbElec = 0;
	while (l) {
		nbElec++;
		l = l->next;
	}
	return nbElec;
}
point* pnext(point* p) { return p->pnext_Connect; }
point* pprec(point* p) { return p->pprec_Connect; }