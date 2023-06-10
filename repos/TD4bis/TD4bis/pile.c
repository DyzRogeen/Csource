#define _CRT_SECURE_NO_WARNINGS 1

#include "pile.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void printList(pile* p_pRoot) {

	if (!p_pRoot) {
		printf("\n");
		return;
	}

	printf("[%d] ", p_pRoot->info);

	printList(p_pRoot->m_next);

}

void push(pile** p_pRoot, int p_data) {

	pile* pHead = (pile*)malloc(sizeof(pile));

	pHead->info = p_data;

	pHead->m_next = *p_pRoot;

	*p_pRoot = pHead;

}

int pop(pile** p_pRoot) {

	if (!(*p_pRoot)) return 0;

	pile* pRet = *p_pRoot;

	*p_pRoot = pRet->m_next;

	int val = pRet->info;

	free(pRet);

	return val;

}