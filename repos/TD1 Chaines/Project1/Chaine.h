#pragma once

int LongueurChaine(char* s);

char* copieChaine(char* s);

int compareChaine(char* s1, char* s2);

char * inverserChaine(char* s);

char* trouverSousChaine(char* s1, char* s);

void libererChaine(char* s);

char* trim(char* s, char c);

char *noCaseSpace(char c);

int palindrome(char* s);

int findFirstOf(char* s1, char* s2);

int findLastOf(char* s1, char* s2);

int findFirstNotOf(char* s1, char* s2);

int findLastNotOf(char* s1, char* s2);

char** split(char* s1, char* s2);

char* toUpper(char* s);

char* inverseCase(char* s);