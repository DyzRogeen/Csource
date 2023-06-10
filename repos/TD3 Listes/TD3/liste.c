#include "liste.h"
#include <stdlib.h>
#include <stdio.h>

void printList(node * p_pRoot) {

	if (!p_pRoot) {
		printf("\n");
		return;
	}

	printf("[%d] ", p_pRoot->m_data);

	printList(p_pRoot->m_next);

}

node* createListNode(int p_value) {

	node * new_Node = (node*)calloc(1, sizeof(node));

	new_Node->m_data = p_value;

	return new_Node;

}

void freeList(node* p_pRoot) {

	if (!p_pRoot) return;
	
	freeList(p_pRoot->m_next);

	free(p_pRoot);

}

node* addFirst(node* p_pRoot, int p_value) {

	node* new_Node = createListNode(p_value);

	new_Node->m_next = p_pRoot;

	return new_Node;

}

node* addLast(node* p_pRoot, int p_value) {


	if (!p_pRoot) return createListNode(p_value);

	node* tmp = p_pRoot;

	while (tmp->m_next != NULL) tmp = tmp->m_next;

	tmp->m_next = createListNode(p_value);;

	return p_pRoot;

}

node* addSortAsc(node* p_pRoot, int p_value) {

	if (!p_pRoot || p_pRoot->m_data > p_value) {

		node* new_Node = createListNode(p_value);
		new_Node->m_next = p_pRoot;
		return new_Node;

	}

	p_pRoot->m_next = addSortAsc(p_pRoot->m_next, p_value);
	return p_pRoot;

}

node* addSortDesc(node* p_pRoot, int p_value) {

	if (!p_pRoot || p_pRoot->m_data < p_value) {

		node* new_Node = createListNode(p_value);
		new_Node->m_next = p_pRoot;
		return new_Node;

	}

	p_pRoot->m_next = addSortDesc(p_pRoot->m_next, p_value);
	return p_pRoot;
}

int listLenght(node* p_pRoot) {

	int i = 0;

	while (p_pRoot != NULL) {
		p_pRoot = p_pRoot->m_next;
		i++;
	}

	return i;

}

node* at(node* p_pRoot, int p_index) {

	for (int i = 0; i < p_index; i++) p_pRoot = p_pRoot->m_next;

	return p_pRoot;

}

node* pop(node* p_pRoot, int * p_result) {

	if (!p_pRoot)return NULL;

	*p_result = p_pRoot->m_data;

	node* tmp = p_pRoot;

	p_pRoot = p_pRoot->m_next;

	//free(tmp);

	return p_pRoot;

}