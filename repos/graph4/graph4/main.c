#define _CRT_SECURE_NO_WARNINGS 1
#include "graph.h"
#include <stdio.h>
#include <stdlib.h>

int main(int argv, char** argc) {
	
	
	graph* g = createGraph();

	printMat(g);

	floyd(g);

	printf("\n");

	printMat(g);
	
	return 0;

}