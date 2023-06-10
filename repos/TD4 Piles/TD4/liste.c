#include "liste.h"
#include <stdio.h>
#include <stdlib.h>

void printList(file* p_pRoot) {

	if (!p_pRoot) {
		printf("\n");
		return;
	}

	printf("[%d] ", p_pRoot->m_data);

	printList(p_pRoot->m_next);

}

void freeList(file* p_pRoot) {

	if (!p_pRoot) return;

	freeList(p_pRoot->m_next);

	free(p_pRoot);

}

void enqueue(file** p_pRoot, int p_valeur) {

	if (!p_pRoot) *p_pRoot = createListNode(p_valeur);

	file* tmp = *p_pRoot;

	while (tmp->m_next != NULL) tmp = tmp->m_next;

	file* new = (file*)malloc(sizeof(file));

	new->m_data = p_valeur;

	tmp->m_next = new;

}

int dequeue(file** p_pRoot);

void push(pile** p_pRoot, int p_data) {

	pile* pHead = (pile*)malloc(sizeof(pile));

	pHead->m_next = *p_pRoot;

	pHead->m_data = p_data;

	*p_pRoot = pHead;

}

int pop(node** p_pRoot) {

	if (!(*p_pRoot)) return 0;
	
	pile* pRet = *p_pRoot;

	*p_pRoot = pRet->m_next;

	int val = pRet -> m_data;

	free(pRet);

	return val;

}