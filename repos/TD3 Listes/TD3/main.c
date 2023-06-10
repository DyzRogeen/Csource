#include "liste.h"
#include <time.h>
#include <stdlib.h>
#include <stdio.h>

void printHanoi(node* col1, node* col2, node* col3, int p_repeat) {

	if (p_repeat == 5) {
		printf("\n");
		return;
	}

	int i;
	if (5 - p_repeat > listLenght(col1)) printf("     |    ");
	else {

		for (i = 0; i <= 5 - col1->m_data; i++) printf(" ");
		for (i = 0; i < col1->m_data * 2 - 1; i++) printf("#");
		for (i = 0; i < 5 - col1->m_data; i++) printf(" ");

	}

	if (5 - p_repeat > listLenght(col2)) printf("     |    ");
	else {

		for (i = 0; i <= 5 - col2->m_data; i++) printf(" ");
		for (i = 0; i < col2->m_data * 2 - 1; i++) printf("#");
		for (i = 0; i < 5 - col2->m_data; i++) printf(" ");

	}

	if (5 - p_repeat > listLenght(col3)) printf("    |");
	else {

		for (i = 0; i < 5 - col3->m_data; i++) printf(" ");
		for (i = 0; i < col3->m_data * 2 - 1; i++) printf("#");

	}

	printf("\n");

	if (4 - p_repeat >= listLenght(col1) && 4 - p_repeat >= listLenght(col2) && 4 - p_repeat >= listLenght(col3)) printHanoi(col1, col2, col3, p_repeat + 1);
	else if (4 - p_repeat >= listLenght(col1) && 4 - p_repeat >= listLenght(col2)) { printHanoi(col1, col2, col3->m_next, p_repeat + 1); return; }
	else if (4 - p_repeat >= listLenght(col2) && 4 - p_repeat >= listLenght(col3)) { printHanoi(col1->m_next, col2, col3, p_repeat + 1); return; }
	else if (4 - p_repeat >= listLenght(col1) && 4 - p_repeat >= listLenght(col3)) { printHanoi(col1, col2->m_next, col3, p_repeat + 1); return; }
	else if (4 - p_repeat >= listLenght(col1)) { printHanoi(col1, col2->m_next, col3->m_next, p_repeat + 1);  return; }
	else if (4 - p_repeat >= listLenght(col2)) { printHanoi(col1->m_next, col2, col3->m_next, p_repeat + 1);  return; }
	else if (4 - p_repeat >= listLenght(col3)) { printHanoi(col1->m_next, col2->m_next, col3, p_repeat + 1);  return; }
	else { printHanoi(col1->m_next, col2->m_next, col3->m_next, p_repeat + 1);  return; }

}

void rhanoi(node** cool1, node** cool2, node** cool3, int hanoi) {
	
	int p_value;
	int* p_result = NULL;
	p_result = &p_value;

	node* col1 = *cool1;
	node* col2 = *cool2;
	node* col3 = *cool3;

	if (hanoi == 1) {
		printf("hanoi = %d\n", hanoi);
		printList(col1);
		printList(col2);
		if (*cool1 && col1->m_data == 1) {
			*cool1 = pop(*cool1, p_result);
			*cool3 = addSortAsc(*cool3, 1);
		}
		else if (*cool2 && col2->m_data == 1) {
			*cool2 = pop(*cool2, p_result);
			*cool1 = addSortAsc(*cool1, 1);
		}
		else if (*cool3 && col3->m_data == 1) {
			*cool3 = pop(*cool3, p_result);
			*cool2 = addSortAsc(*cool2, 1);
		}printHanoi(*cool1, *cool2, *cool3, 0);
		return;
	}

	rhanoi(&col1, &col2, &col3, hanoi - 1);
	printf("hanoi = %d\n", hanoi);
	printList(col1);
	printList(col2);
	if (hanoi % 2 == 0) {
		if (col1 && col1->m_data == hanoi) {
			col1 = pop(col1, p_result);
			col2 = addSortAsc(col2, hanoi);
		}else if (col2 && col2->m_data == hanoi) {
			col2 = pop(col2, p_result);
			col3 = addSortAsc(col3, hanoi);
		}else if (col3 && col3->m_data == hanoi) {
			col3 = pop(col3, p_result);
			col1 = addSortAsc(col1, hanoi);
		}
	}
	else {
		if (col1 && col1->m_data == hanoi) {
			col1 = pop(col1, p_result);
			col3 = addSortAsc(col3, hanoi);
		}
		else if (col2 && col2->m_data == hanoi) {
			col2 = pop(col2, p_result);
			col1 = addSortAsc(col1, hanoi);
		}
		else if (col3 && col3->m_data == hanoi) {
			col3 = pop(col3, p_result);
			col2 = addSortAsc(col2, hanoi);
		}
	}

	printHanoi(col1, col2, col3, 0);
	rhanoi(&col1, &col2, &col3, hanoi - 1);
	return;

	printHanoi(col1, col2, col3, 0);
}

void hanoi(node** cool1, node** cool2, node** cool3) {

	int p_value;
	int* p_result = NULL;
	p_result = &p_value;
	int col = 1;

	node* col1 = *cool1;
	node* col2 = *cool2;
	node* col3 = *cool3;

	while (listLenght(col3) != 5) {

		if (col1 && col == 1) {

			if (!col3 || col1->m_data < col3->m_data) {

				col1 = pop(col1, p_result);
				col3 = addSortAsc(col3, *p_result);
				if (col2) col = 2;

			}
			else if (!col2 || col1->m_data < col2->m_data) {

				col1 = pop(col1, p_result);
				col2 = addSortAsc(col2, *p_result);
				if (col3) col = 3;

			}
			else { col = 3; }

			printHanoi(col1, col2, col3, 0);

		}

		if (col3 && col == 3) {

			if (!col2 || col3->m_data < col2->m_data) {

				col3 = pop(col3, p_result);
				col2 = addSortAsc(col2, *p_result);
				if (col1) col = 1;

			}
			else if (!col1 || col3->m_data < col1->m_data) {

				col3 = pop(col3, p_result);
				col1 = addSortAsc(col1, *p_result);
				if (col2) col = 2;

			}
			else { col = 2; }

			printHanoi(col1, col2, col3, 0);

		}

		if (col2 && col == 2) {

			if (!col1 || col2->m_data < col1->m_data) {

				col2 = pop(col2, p_result);
				col1 = addSortAsc(col1, *p_result);
				if (col3) col = 3;

			}
			else if (!col3 || col2->m_data < col3->m_data) {

				col2 = pop(col2, p_result);
				col3 = addSortAsc(col3, *p_result);
				if (col1) col = 1;

			}
			else { col = 1; }

			printHanoi(col1, col2, col3, 0);

		}

	}

}

int main(int argc, char** argv) {

	node* col1 = NULL;
	node* col2 = NULL;
	node* col3 = NULL;
	srand(time(NULL));
	for (int i = 0; i < 5; i++) col1 = addLast(col1, i + 1);

	printHanoi(col1, col2, col3, 0);
	rhanoi(&col1, &col2, &col3, 5);

	/*
	printList(col1);
	printList(col2);
	printList(col3);

	col1 = pop(col1, p_result);
	col2 = addSortAsc(col2, *p_result);
	col1 = pop(col1, p_result);
	col3 = addSortAsc(col3, *p_result);
	col2 = pop(col2, p_result);
	col3 = addSortAsc(col3, *p_result);
	col1 = pop(col1, p_result);
	col2 = addSortAsc(col2, *p_result);
	col3 = pop(col3, p_result);
	col1 = addSortAsc(col1, *p_result);
	col3 = pop(col3, p_result);
	col2 = addSortAsc(col2, *p_result);
	col1 = pop(col1, p_result);
	col2 = addSortAsc(col2, *p_result);

	printf("\n");
	printList(col1);
	printList(col2);
	printList(col3);
	*/

	freeList(col1);
	freeList(col2);
	freeList(col3);
	return 0;

}