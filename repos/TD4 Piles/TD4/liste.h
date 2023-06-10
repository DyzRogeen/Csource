#pragma once

typedef struct Node {

	int m_data;

	struct Node* m_next;

}node;

typedef struct sPile {

	int m_data;

	struct sPile* m_next;

}pile;

typedef struct sFile {

	int m_data;

	struct sFile* m_next;

}file;

void printList(file* p_pRoot);

void freeList(file* p_pRoot);

void enqueue(file** p_pRoot, int p_valeur);

int dequeue(file** p_pRoot);

void push(pile** p_pRoot, int p_data);

int pop(pile** p_pRoot);