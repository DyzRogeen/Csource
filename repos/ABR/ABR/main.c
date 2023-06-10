#include <stdlib.h>
#include <stdio.h>
#include "ABR.h"

int main(int argc, char** argv) {

	abr* tree = createNode(7);
	addNode(&tree, 3);
	addNode(&tree, 11);
	addNode(&tree, 2);
	addNode(&tree, 5);
	addNode(&tree, 6);
	addNode(&tree, 10);
	affTree(tree);
	supprNode(&tree, 7);
	affTree(tree);
	printf("Noeud %d : F = %d\n", (tree)->m_data, F(tree));
	//printf("%d\n", (*findNode(tree, 4))->m_data);
	return 0;

}