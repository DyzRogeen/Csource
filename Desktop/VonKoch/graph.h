#pragma once

#include <stdio.h>

typedef struct sItem {
	int isWindow, isFile;
	int depth;
	struct sItem* children;
};