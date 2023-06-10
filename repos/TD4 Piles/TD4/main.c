#include "liste.h"
#include <stdio.h>
#include <stdlib.h>

int main() {

	file* pHead = (file*)malloc(sizeof(file));
	for (int i = 0; i < 10; i++) enqueue(&pHead, i + 1);
	printList(pHead);
	freeList(pHead);

}