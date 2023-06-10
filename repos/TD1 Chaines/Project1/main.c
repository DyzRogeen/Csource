#include <stdio.h>
#include "Chaine.h"

int main() {
    char* s = toUpper("saLouTe BilouTe");
    printf("%s\n", s);
    char* t = inverseCase("saLouTe BilouTe");
    printf("%s\n", t);
    return 0;
}