#include "elec.h"

point* createPoint(point p) { // TODO SUPPR DEPENDANCE A L
	point* pp = (point*)calloc(1, sizeof(point));
	pp->pnext_Connect = NULL;
	pp->pprec_Connect = NULL;
	pp->x = p.x;
	pp->y = p.y;
	pp->alt = 0;
	pp->step = RESET;
	pp->V = 0;
	pp->id = -1;

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
	e->I = 0; // A
	e->U = 0; // V
	e->amplU = 0;
	e->selected = 1;
	if (t == GENERATEUR) {
		e->U = e->amplU = 12;
		e->p1->V = -e->U / 2;
		e->p2->V =  e->U / 2;
		e->Freq = 0; // Hz
	}
	if (t == RESISTANCE) e->R = 1000; // O
	if (t == CONDENSATEUR) e->C = 0.0001; // F
	if (t == BOBINE) e->L = 100; // H

	return e;
}
list* createList(elec* e) {
	list* l = (list*)calloc(1, sizeof(list));
	l->e = e;
	e->l = l;
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
	// TODO : FUSE WIRES
	/*if (e->t == WIRE) {
		point* neigh = neighborSwitch(e->p1);
		if (cntConnectedPoles(e->p1) == 1 && neigh->e->t == WIRE) {
			
		}
	}*/
	list* tmp = createList(e);
	tmp->next = *l;
	(*l)->prec = tmp;
	*l = tmp;
}
void deleteSelected(list** l) {
	
	if (!l || !*l) return;
	elec* e = (*l)->e;

	if (!e->selected) {
		deleteSelected(&(*l)->next);
		return;
	}
		
	freeElec(e);
	(*l)->e = NULL;

	list* tmp = *l;

	if (tmp->prec) tmp->prec->next = tmp->next;
	if (tmp->next) {
		tmp->next->prec = tmp->prec;
		*l = tmp->next;
		deleteSelected(l);
	}

	free(tmp);
	// TODO : FIX *l = NULL;
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
	const char* enumToString[8] = { "VCC", "GND", "GENERATEUR", "RESISTANCE", "BOBINE", "CONDENSATEUR", "DIODE", "WIRE"};
	printf("[%s | U : %.1fV (p1:%.1fV,p2:%.1fV) | I : %.1fmA | R : %.1fO", enumToString[e->t], e->U, e->p1->V, e->p2->V, e->I * 1000, e->R);
	if (e->t == CONDENSATEUR) printf(" | q : %.1fC | C : %.1fF", e->C);
	printf("]\n");
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
void freeElec(elec* e) {
	disconnect(e->p1);
	disconnect(e->p2);
	free(e);
}

/*list* simulate(list* l) {

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
}*/

// TODO : OPTI les while avec des listes préfaites selon le type
void initV(list* l, int alt) {
	elec* e;
	int GNDorVCCfound = 0;

	// INIT VCC / GND
	list* ltmp = l;
	while (ltmp) {
		e = ltmp->e;
		if (e->t > GND) {
			ltmp = ltmp->next;
			continue;
		}

		GNDorVCCfound = 1;

		e->p1->V = e->t == VCC ? 5 : 0;

		if (propagateV(e->p1, alt) == 0);// printf("COURT CIRCUIT !\n");
		
		ltmp = ltmp->next;
	}

	if (1 && GNDorVCCfound) return;

	// Générateurs et autres sources (si GND ou VCC non trouvés)
	ltmp = l;
	while (ltmp) {
		e = ltmp->e;
		if (e->t != GENERATEUR) {
			ltmp = ltmp->next;
			continue;
		}

		e->p1->V = 0;
		e->p2->V = e->U;

		if (propagateV(e->p1, alt) == 0 /* || propagateV(e->p2, alt) == 0*/) printf("COURT CIRCUIT !\n");

		ltmp = ltmp->next;
	}

	return;

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

		if (e->Freq == 0 && p1->alt == alt && p2->alt == alt && p2->V - p1->V != e->U) printf("GENERATEUR FORCE !\n");
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
int propagateV(point* p, int alt) {
	if (p->alt == alt && p->step == INIT_V) return 1;
	p->alt = alt;
	p->step = INIT_V;

	int tmp = propagate(p, alt, 1);
	return propagate(p, alt, -1) && tmp;
}
int propagate(point* p, int alt, int direction) {

	float V = p->V;
	elec* e = p->e, * e_;

	point* pTmp = direction == 1 ? p->pnext_Connect : p->pprec_Connect , * pProp;
	type t;
	int ret = 1;
	while (pTmp) {
		if (pTmp->alt == alt) return 0;
		pTmp->alt = alt;
		pTmp->step = INIT_V;

		e_ = pTmp->e;
		t = e_->t;
		if (t == GND) return V == 0;
		if (t == VCC) return V != 0;
		pTmp->V = V;

		switch (t) {
		case WIRE:
			e_->U = 0;
			break;
		case CONDENSATEUR:
			e_->amplU = e_->U = e_->q / e_->C;
			break;
		}

		if (t == WIRE /* || t == GENERATEUR || t == CONDENSATEUR*/) { // Commenté pour MNA
			pProp = poleSwitch(pTmp);
			pProp->V = (t == GENERATEUR ? -1 : 1) * (pProp == e_->p1 ? 1 : -1) * e_->U + V;
			ret = propagateV(pProp, alt) && ret;
		}

		pTmp = direction == 1 ? pTmp->pnext_Connect : pTmp->pprec_Connect;
	}

	return ret;
}

//int propagateBackward(point* p, int alt) {
//
//	float V = p->V;
//	elec* e = p->e, * e_;
//
//	point* pTmp = p->pprec_Connect, * pProp;
//	type t;
//	int ret = 1;
//	while (pTmp) {
//		if (pTmp->alt == alt) return 0;
//		pTmp->alt = alt;
//
//		e_ = pTmp->e;
//		t = e_->t;
//		if (t == GND) return V == 0;
//		if (t == VCC) return V != 0;
//		pTmp->V = V;
//		if (t == WIRE) {
//			pProp = poleSwitch(pTmp);
//			pProp->V = V;
//			ret = propagateV(pProp, alt) && ret;
//		}
//		else if (t == GENERATEUR) {
//			pProp = poleSwitch(pTmp);
//			pProp->V = (pProp == e_->p1 ? -1 : 1) * e_->U + V;
//			ret = propagateV(pProp, alt) && ret;
//		}
//		pTmp = pTmp->pprec_Connect;
//	}
//
//	return ret;
//}

void simuI(list* l, int alt) {
	elec* e;

	list* ltmp = l;
	while (ltmp) {
		e = ltmp->e;

		propI(e->p1, e->p1->V, 0, alt);

		ltmp = ltmp->next;
	}
}
float propI(point* p, float V, float R, int alt) {

	if (p->alt == alt) return;

	float Veq;
	
	if (p->e->t <= GND || p->alt == 3 && alt == 4 || p->alt == -3 && alt == -4) Veq = p->V;	// Si le potentiel est set, on le met.
	else p->V = Veq = getVeq(p, V, R, alt);												    // Sinon, on va le chercher

	float sumI = 0;// propIF(p, Veq, 0, alt);
	p->alt = alt;

	point* p_tmp = p->pnext_Connect;
	p_tmp = p->pnext_Connect;
	while (p_tmp) {
		p_tmp->V = Veq;
		sumI += propIF(p_tmp, Veq, 0, alt);
		p_tmp->alt = alt; p_tmp = p_tmp->pnext_Connect;
	}
	p_tmp = p->pprec_Connect;
	while (p_tmp) {
		p_tmp->V = Veq;
		sumI += propIF(p_tmp, Veq, 0, alt);
		p_tmp->alt = alt; p_tmp = p_tmp->pprec_Connect;
	}

	return sumI;

}
float getVeq(point* p, float V, float R, int alt) {

	float Vn[4] = { V, 0, 0, 0 };
	float Rn[4] = { R, 0, 0, 0 };

	int i = 1;
	point* p_tmp = p->pnext_Connect;
	while (p_tmp) {
		fetchVR(p_tmp, Vn + i, Rn + i, alt);
		p_tmp = p_tmp->pnext_Connect; i++;
	}
	p_tmp = p->pprec_Connect;
	while (p_tmp) {
		fetchVR(p_tmp, Vn + i, Rn + i, alt);
		p_tmp = p_tmp->pprec_Connect; i++;
	}

	// Loi des mailles
	if (i == 4)
		return (
		Rn[1] * Rn[2] * Rn[3] * Vn[0] +
		Rn[0] * Rn[2] * Rn[3] * Vn[1] +
		Rn[0] * Rn[1] * Rn[3] * Vn[2] +
		Rn[0] * Rn[1] * Rn[2] * Vn[3]
		) / (
		Rn[1] * Rn[2] * Rn[3] +
		Rn[0] * Rn[2] * Rn[3] +
		Rn[0] * Rn[1] * Rn[3] +
		Rn[0] * Rn[1] * Rn[2]);
	if (i == 3)
		return (
		Rn[1] * Rn[2] * Vn[0] +
		Rn[0] * Rn[2] * Vn[1] +
		Rn[0] * Rn[1] * Vn[2]
		) / (
		Rn[1] * Rn[2] +
		Rn[0] * Rn[2] +
		Rn[0] * Rn[1]);
	if (i == 2)
		return (
		Rn[1] * Vn[0] +
		Rn[0] * Vn[1]
		) / (Rn[0] + Rn[1]);
	if (i == 1) return V;

}
void fetchVR(point* p, float* V, float* R, int alt) {
	if (!p) return;
	if (p->e->t <= GND || p->alt == 3 && alt == 4 || p->alt == -3 && alt == -4) {
		*V += p->V;
		return;
	}

	// On traverse le dipôle
	p = poleSwitch(p);

	if (p->e->amplU != 0) *V += (p == p->e->p1 ? 1 : -1) * p->e->U;
	else *R += p->e->R;	// On accumule les R

	if (isBifurc(p)) {
		// TODO
	}
	else {
		fetchVR(neighborSwitch(p), V, R, alt);
	}

}
float propIF(point* p, float V, float R, int alt) {
	if (!p) return 0;
	if (p->alt == alt) return p->e->I;

	elec* e = p->e;
	if (e->t <= GND) return 0; // Si VCC ou GND, on ignore

	float Vtmp = p->V;

	// On traverse le dipôle
	p = poleSwitch(p);
	R += e->R;			// On accumule les R ...
	if (e->amplU != 0)	// Et les V si le dipôle génère de la tension
		V += (p->V > Vtmp ? 1 : -1) * e->U;

	float I;
	// Autre potentiel set trouvé
	if ((e->t <= GND || p->alt == alt || p->alt == 3 && alt == 4 || p->alt == -3 && alt == -4) && R != 0) I = (p->V - V) / R;
	// Sinon on continue
	else if (isBifurc(p)) I = propI(p, V, R, alt); // Cas bifurcation
	else I = propIF(neighborSwitch(p), V, R, alt); // Cas série

	if (p->alt == alt) return I;

	p->alt = alt;
	e->I = fabs(I);// * (e->p1->V > e->p1->V ? -1 : 1) * (e->t == GENERATEUR ? -1 : 1);

	Vtmp = p->V;
	p = poleSwitch(p);

	if (p->alt == 3 && alt == 4 || p->alt == -3 && alt == -4) {
		p->alt = alt;
		return I;
	}

	// On calcule les potentiels avec l'intensité trouvée
	if (e->amplU != 0) p->V = (p == e->p1 ? -1 : 1) * e->U + Vtmp;
	else {
		e->U = I * e->R;
		p->V = Vtmp - e->U;
	}

	propV(p);
	
	p->alt = alt;

	return I;

}
void propV(point* p) {
	float V = p->V;

	point* ptmp = p->pnext_Connect;
	while (ptmp) {
		ptmp->V = V;
		ptmp = ptmp->pnext_Connect;
	}
	ptmp = p->pprec_Connect;
	while (ptmp) {
		ptmp->V = V;
		ptmp = ptmp->pprec_Connect;
	}

}

/*
void initI(list* l, int alt) {
	list* ltmp = l; elec* e; point* p1, * p2;
	while (ltmp) {
		e = ltmp->e;
		// On initialise ceux pour quoi on peut calculer I.
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
	}/
}
void propagateI(point* p, float I, int alt) {
	if (!p || p->alt == alt) return;
	p->alt = alt;
	I = p->e->t == WIRE ? -I : I;
	p->e->I = I;
	p = poleSwitch(p);
	if (!isBifurc(p)) propagateI(neighborSwitch(p), I, alt);
	else nodeSum(p, alt);
}*/
/*/ Loi des mailles
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

}*/
float enterOrExitNode(point* p) {
	elec* e = p->e;
	return e->I * (e->p1->V > e->p2->V ? -1 : 1) * (e->t == GENERATEUR ? -1 : 1);
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

void resetElecs(list* l) {
	elec* e;
	while (l) {
		e = l->e;

		if (e->t == CONDENSATEUR || e->t == BOBINE) {
			e->amplU = e->U = e->I = e->q = 0;
			e->p1->V = 0;
			e->p2->V = 0;
		}

		l = l->next;
	}
}

point* neighborSwitch(point* p) {
	return p->pnext_Connect ? p->pnext_Connect : p->pprec_Connect;
}
point* poleSwitch(point* p) {
	elec* e = p->e;
	return p == e->p1 ? e->p2 : e->p1;
}

int cntConnectedPoles(point* p) {
	int cnt = 0;

	point* pTmp = p->pnext_Connect;
	while (pTmp) {
		cnt++;
		pTmp = pTmp->pnext_Connect;
	}
	pTmp = p->pprec_Connect;
	while (pTmp) {
		cnt++;
		pTmp = pTmp->pprec_Connect;
	}
	return cnt;
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


// NOUVELLE METHODE : MODIFIED NODAL ANALISIS, résolution par matrices de circuit (à creuser voir paint que j'ai fait dessus)

/**
* 1. Déterminer les potentiels et intensités inconnus, attribuer les ids.
* 2. Construire la matrice MNA à partir des conductances des dipôles du circuit.
* 3. Résoudre le système et attribuer les potentiels aux noeuds correspondants.
* 4. Propager les courants trouvés aux dipôles.
*/

// Compte le nombre de potentiels inconnus
int countNodes(list* l) {
	resetIds(l);
	initV(l, 2); // TODO : faire nouvelle version.

	int nb_nodes = 0;

	elec* e;
	while (l) {
		e = l->e;
		if (e->t < GENERATEUR) {
			l = l->next;
			continue;
		}
		if (e->t == GENERATEUR || e->t == BOBINE) e->id = nb_nodes++;
		setNodeId(e->p1, &nb_nodes, 1);
		setNodeId(e->p2, &nb_nodes, 1);
		l = l->next;
	}

	return nb_nodes;
}

void resetIds(list* l) {
	// Reset des Ids
	elec* e;
	point* p1, * p2;
	while (l) {
		e = l->e;
		e->id = -1;
		p1 = e->p1; p2 = e->p2;
		p1->id = -1;
		p2->id = -1;
		if (1 || e->t <= GENERATEUR) {
			p1->step = RESET;
			p2->step = RESET;
			p1->alt = -1;
			p2->alt = -1;
			
		}
		l = l->next;
	}
}


// Set l'id de tous les pôles du noeud
void setNodeId(point* p, int* nb_nodes, int increment) {
	if (p->id  != -1) return; // Si le noeud a déjà un id, on passe.
	if (p->step == INIT_V) return; // Si l'on connaît son potentiel, on ne compte pas le noeud.

	int id = p->id = *nb_nodes;

	//printf("V%d => ( %.1f ; %.1f )\n", id, p->x, p->y);

	if (p->e->t == WIRE && increment) setNodeId(poleSwitch(p), nb_nodes, 0);
	
	point* ptmp = p->pnext_Connect;
	while (ptmp) {
		ptmp->id = id;
		if (ptmp->e->t == WIRE) setNodeId(poleSwitch(ptmp), nb_nodes, 0);
		ptmp = ptmp->pnext_Connect;
	}
	ptmp = p->pprec_Connect;
	while (ptmp) {
		ptmp->id = id;
		if (ptmp->e->t == WIRE) setNodeId(poleSwitch(ptmp), nb_nodes, 0);
		ptmp = ptmp->pprec_Connect;
	}

	*nb_nodes += increment;
}


float* buildMNAMatrix(list* l, int nb_nodes, float dt) {
	
	float* M = (float*)calloc(nb_nodes * nb_nodes, sizeof(float));
	float* F = (float*)calloc(nb_nodes, sizeof(float));

	elec* e;
	point *p1, *p2;
	while (l) {

		e = l->e;
		if (e->t < GENERATEUR) {
			l = l->next;
			continue;
		}

		p1 = e->p1;
		p2 = e->p2;

		// Recherche des potentiels à définir
		fillMNARow(p1, M, F, nb_nodes, dt);
		fillMNARow(p2, M, F, nb_nodes, dt);

		if ((e->t == GENERATEUR || e->t == BOBINE) && e->id != -1) {
			float* row = M + e->id * nb_nodes;
			switch (e->t) {
			case GENERATEUR:

				if (p1->id != -1) row[p1->id] = 1;
				else if (p1->step == INIT_V) F[e->id] -= p1->V;

				if (p2->id != -1) row[p2->id] = -1;
				else if (p2->step == INIT_V) F[e->id] += p2->V;

				F[e->id] += -e->U;

				break;
			case BOBINE:
			{
				float Ldt = e->L / dt;

				if (p1->id != -1) row[p1->id] = 1;
				else if (p1->step == INIT_V) F[e->id] += p1->V;

				if (p2->id != -1) row[p2->id] = -1;
				else if (p2->step == INIT_V) F[e->id] -= p2->V;
				F[e->id] -= e->I * Ldt;

				row[e->id] = Ldt;
				break;
			}
			}
			
		}

		l = l->next;
	}

	
	/*printMatrix(M, nb_nodes);

	printf("[ ");
	for (int i = 0; i < nb_nodes; i++) printf(F[i] < 0 ? "%.3f " : " %.3f ", F[i]);
	printf(" ]\n\n");*/

	float* M_inv = inverse(M, nb_nodes);

	/*printMatrix(M_inv, nb_nodes);*/

	float* f = matrixVectorProduct(M_inv, F, nb_nodes);

	free(M);
	if (M_inv) free(M_inv);
	free(F);

	/*if (f) for (int i = 0; i < nb_nodes; i++) printf( "%.3f ", f[i]);
	printf("\n\n");*/

	return f;

}

void fillMNARow(point* p, float* M, float* F, int nb_nodes, float dt) {
	if (p->step == MNA) return; // Si le noeud est déjà traité, on passe.
	if (p->step == INIT_V) return; // Si l'on connaît son potentiel, on ne compte pas le noeud.

	// Potentiel pas encore identifié, on associe l'id au pôle et on incrémente l'id max.
	int id = p->id;

	// Pour chaque dipôle rattaché au noeud, on remplit la ligne de la matrice MNA.
	float* row = M + id * nb_nodes;
	handlePole(p, id, row, F, dt);

	point* ptmp = p->pnext_Connect;
	while (ptmp) {
		handlePole(ptmp, id, row, F, dt);
		ptmp = ptmp->pnext_Connect;
	}
	ptmp = p->pprec_Connect;
	while (ptmp) {
		handlePole(ptmp, id, row, F, dt);
		ptmp = ptmp->pprec_Connect;
	}

}

void handlePole(point* p, int id, float* row, float* F, float dt) {
	p->step = MNA;
	elec* e = p->e;

	switch (e->t) {
	case WIRE: return;
	case GENERATEUR:
	{
		// Cas particulier, la logique est la même mais on indique maintenant le sens du courant. (et F pointe sur la ligne du générateur)
		int dir = (p == e->p1 ? 1 : -1);
		row[e->id] += dir;

		//point* p2 = poleSwitch(p);
		//if (p2->step == INIT_V) F[e->id] += p2->V * dir;

		return;
	}
	case CONDENSATEUR:
	{
		float Cdt = e->C / dt;

		// On discrétise la relation entre le courant, la capacité et la tension
		// In = Cn * dUn / dt avec Un = (v - v') et dUn = v - v[t-1] - v' + v'[t-1]
		row[id] += Cdt;

		point* p2 = poleSwitch(p);
		
		if (p2->step == INIT_V) F[id] += p->V * Cdt; // v' constant donc dv' = 0
		else {
			row[p2->id] -= Cdt;
			F[id] += (p->V - p2->V) * Cdt;
		}
		break;
	}
	case BOBINE:
	{

		int dir = (p == e->p1 ? -1 : 1); // TODO : sens du courant ?


		// On discrétise la relation entre le courant, l'inductance et la tension
		// dIn / dt = Un / L avec Un = (v - v') et dIn = In - In[t-1] => In = (v - v') * dt / L + In[t-1]
		row[e->id] += dir;

		//float Ldt = dt / e->L;
		//point* p2 = poleSwitch(p);
		//if (p2->step == INIT_V) F[e->id] += dir * (p->V * Ldt + e->I);
		//else F[e->id] += dir * e->I;
		
		break;
	}
	default:
	{
		float G = 1.f / e->R;

		// On ajoute la conductance à la colonne du potentiel et on la soustrait à celle du potentiel du second pôle.
		// In = Gn * Un avec Un = (v - v')
		row[id] += G;

		point* p2 = poleSwitch(p);
		if (p2->step == INIT_V) F[id] += p2->V * G;
		else row[p2->id] -= G;
	}
	}

}


void setVoltage(list* l, float* F, float dt) {

	elec* e;
	point *p1, *p2;
	list* ltmp = l;
	while (ltmp) {
		e = ltmp->e;

		if (e->t < GENERATEUR) {
			ltmp = ltmp->next;
			continue;
		}

		p1 = e->p1;
		p2 = e->p2;

		if (p1->id != -1) setNodePotential(p1, F[p1->id]);
		if (p2->id != -1) setNodePotential(p2, F[p2->id]);

		switch (e->t) {
		case GENERATEUR:
			e->I = F[e->id];
			break;
		case CONDENSATEUR:
		{
			float dU = p2->V - p1->V - e->U;
			e->U = p2->V - p1->V;
			e->I = e->C * dU / dt;
			break;
		}
		case BOBINE:
		{
			e->U = p2->V - p1->V;
			e->I = dt * e->U / e->L + e->I;
			break;
		}
		case WIRE:// TODO : Gérer les courants des fils ici ou ailleurs ?
		{
			float sumI = 0;
			point* ptmp = p1->pnext_Connect;
			while (ptmp) {
				sumI += ptmp->e->I;
				ptmp = ptmp->pnext_Connect;
			}
			ptmp = p1->pprec_Connect;
			while (ptmp) {
				sumI += ptmp->e->I;
				ptmp = ptmp->pprec_Connect;
			}
			e->I = -sumI;
			break;
		}
		default:
			e->U = p2->V - p1->V; // TODO : laisser ca là ? Définir signe ?
			e->I = e->U / e->R;
		}
	
		ltmp = ltmp->next;
	}

	free(F);

}

void setNodePotential(point* p, float V) {
	if (p->step == SET_VOLTAGE) return;
	if (p->step == INIT_V) return;

	p->V = V;
	p->step = SET_VOLTAGE;
	if (p->e->t == WIRE) setNodePotential(poleSwitch(p), V);

	point* ptmp = p->pnext_Connect;
	while (ptmp) {
		ptmp->V = V;
		ptmp->step = SET_VOLTAGE;
		if (ptmp->e->t == WIRE) setNodePotential(poleSwitch(ptmp), V);
		ptmp = ptmp->pnext_Connect;
	}
	ptmp = p->pprec_Connect;
	while (ptmp) {
		ptmp->V = V;
		ptmp->step = SET_VOLTAGE;
		if (ptmp->e->t == WIRE) setNodePotential(poleSwitch(ptmp), V);
		ptmp = ptmp->pprec_Connect;
	}

}


void printMatrix(float* M, int size) {

	if (!M) return;

	float mTmp;
	for (int i = 0; i < size; i++) {
		printf("[ ");
		for (int j = 0; j < size; j++) {
			mTmp = M[j + i * size];
			printf( mTmp < 0 ? "%.3f " : " %.3f ", mTmp);
		}
		printf(" ]\n");
	}
	printf("\n");
}

float* matrixVectorProduct(float* M, float* v, int size) {

	if (!M) return NULL;
	float* f = (float*)calloc(size, sizeof(float));

	for (int i = 0; i < size; i++) {
		f[i] = 0;
		for (int j = 0; j < size; j++)
			f[i] += M[j + i * size] * v[j];
	}
	return f;
}

float* inverse(float* M, int dim) {

	float d = 0, det_res;

	float* M_inv = (float*)calloc(dim * dim, sizeof(float));

	int* skip_col = (int*)calloc(dim, sizeof(int));

	for (int j = 0; j < dim; j++)
		for (int i = 0; i < dim; i++) {
			skip_col[0] = i;

			det_res = ((i + j) % 2 ? 1 : -1) * det(M, dim, j, skip_col, 0);
			if (j == 0) d += M[i + j * dim] * det_res;

			M_inv[i + j * dim] = det_res;
		}

	free(skip_col);

	if (d == 0) {
		free(M_inv);
		return NULL;
	}

	scaleMatrix(M_inv, dim * dim, 1.f / d);

	return M_inv;

}


float det(float* M, int dim, int skip_row, int* skip_col, int row) {

	if (skip_row == row) row++;
	if (row == dim) return 1;

	float d = 0;
	int cnt = 0, not_row_skipped = (skip_row > row);

	for (int i = 0; i < dim; i++) {
		if (isInArray(i, skip_col, row + not_row_skipped)) continue;

		skip_col[row + not_row_skipped] = i;

		d += (cnt % 2 ? 1 : -1) * M[i + row * dim] * det(M, dim, skip_row, skip_col, row + 1);
		cnt++;
	}

	return d;

}

//float det(float* M, int dim, int skip_col, int skip_row) {
//
//	if (dim == 2) return M[0] * M[3] - M[1] * M[2];
//
//	float d = 0, product;
//	int col, row, skip = skip_col != -1 || skip_row != -1;
//	// Diagonales positives
//	for (int i = 0; i < dim - skip; i++) {
//
//		product = 1;
//		col = i; row = 0;
//		for (int j = 0; j < dim - skip; j++) {
//
//			if (col % dim == skip_col) col++;
//			if (row == skip_row) row++;
//
//			product *= M[col % dim + row * dim];
//			col++; row++;
//
//		}
//
//		d += product;
//	}
//	// Diagonales Négatives
//	for (int i = 0; i < dim - skip; i++) {
//
//		product = -1;
//		col = i; row = 0;
//		for (int j = 0; j < dim - skip; j++) {
//
//			if (col < 0) col += dim;
//
//			if (col == skip_col) col--;
//			if (row == skip_row) row++;
//
//			if (j != skip_row) product *= M[col + row * dim];
//			col--; row++;
//		}
//
//		d += product;
//	}
//	return d;
//}

int isInArray(int val, int* array, int size) {
	for (int i = 0; i < size; i++)
		if (array[i] == val) return 1;
	return 0;
}

void scaleMatrix(float* M, int size, float val) {
	for (int i = 0; i < size; i++) M[i] *= val;
}