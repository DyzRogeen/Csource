#pragma once

typedef struct sPile {

	void* info;

	struct sPile* m_next;

}pile;

void printList(pile* p_pRoot);

void push(pile** p_pRoot, int p_data);

int pop(pile** p_pRoot);