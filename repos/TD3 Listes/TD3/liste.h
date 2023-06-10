#pragma once

typedef struct sNode {

	int m_data;

	struct sNode * m_next;

}node;

void printList(node* p_pRoot);

node * createListNode(int p_value);

void freeList(node* p_pRoot);

node* addFirst(node* p_pRoot, int p_value);

node* addLast(node* p_pRoot, int p_value);

node* addSortAsc(node* p_pRoot, int p_value);

node* addSortDesc(node* p_pRoot, int p_value);

int listLenght(node* p_pRoot);

node* at(node* p_pRoot, int p_index);

node* pop(node* p_pRoot, int * p_result);