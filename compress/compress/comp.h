#pragma once

typedef struct Slist {

	int m_data;
	struct Slist* m_next;

}list;

typedef struct Stree {

	int m_data;
	struct Stree* child[2];

}tree;

char* RLE(char* s);