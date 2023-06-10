#include <stddef.h>

int LongueurChaine(char* s) {
    int i = 0;
    while (s[i]!='\0') i++;
    return i;
}

char* copieChaine(char* s) {
    if (!s) return NULL;
    int lon = LongueurChaine(s);
    char *s1 = (char*)calloc(lon + 1, sizeof(char));
    for (int i = 0;i<lon;i++) {
        s1[i] = s[i];
    }return s;
}

int compareChaine(char* s1, char* s2) {
    if (!s1 || !s2) return 0;
    if (LongueurChaine(s1) == LongueurChaine(s2)) {
        int i;
        for (i = 0;i < LongueurChaine(s1);i++) {
            if (s1[i] != s2[i]) return 0;
        } return 1;
    }return 0;
}

char * inverserChaine(char* s) {
    if (!s) return NULL;
    int lon = LongueurChaine(s);
    char* s1 = (char*)calloc(lon + 1, sizeof(char));
    for (int i = 0;i < lon;i++) {
        s1[i] = s[lon - i - 1];
    }return s1;
}

char* trouverSousChaine(char* s1, char* s) {
    if (!s || !s1) return NULL;
    int ccnt = 0;
    int lon = LongueurChaine(s1);
    for (int i = 0; i <= LongueurChaine(s) - lon; i++) {
        ccnt = 0;
        for (int j = 0; s1[j] == s[i + j] && j < LongueurChaine(s); j++)ccnt++;
        if (ccnt == lon) return s + i;
    }return NULL;
}

void libererChaine(char* s) {
    if (s) free(s);
}

char* trim(char* s, char c) {
    int ccnt = 0;
    int lon = LongueurChaine(s);
    for (int i = 0; i < lon; i++) {
        if (s[i] == c)ccnt++;
    }
    char* s1 = (char*)calloc(lon - ccnt,sizeof(char));
    ccnt = 0;
    for (int i = 0; i < lon; i++){
        if (s[i] != c) s1[ccnt++] = s[i];
    }s1[ccnt] = '\0';
    return s1;
}

char* noCaseSpace(char* s) {
    int lon = LongueurChaine(s);
    int ccnt = 0;
    char *s1 = (char*)calloc(lon, sizeof(char));
    for (int i = 0; i < lon; i++) {
        if ('A' <= s[i] && s[i] <= 'Z') s1[ccnt++] = s[i] + 'a' - 'A';
        if ('a' <= s[i] && s[i] <= 'z') s1[ccnt++] = s[i];
    }s1[ccnt] = '\0';
    return s1;
}

int palindrome(char* s) {
    char* s1 = noCaseSpace(s);
    int lon = LongueurChaine(s1);
    for (int i = 0; i < lon / 2; i++) {
        if (s1[i] != s1[lon - i - 1]) return 0;
    }return 1;
}

int findFirstOf(char* s1, char* s2) {
    for (int i = 0; i < LongueurChaine(s1); i++) {
        for (int j = 0; j < LongueurChaine(s2); j++) {
            if (s1[i] == s2[j]) return i;
        }
    }return -1;
}

int findLastOf(char* s1, char* s2) {
    for (int i = LongueurChaine(s1) - 1; i >= 0; i--) {
        for (int j = 0; j < LongueurChaine(s2); j++) {
            if (s1[i] == s2[j]) return i;
        }
    }return -1;
}

int findFirstNotOf(char* s1, char* s2) {
    for (int i = 0; i < LongueurChaine(s1); i++) {
        for (int j = 0; j < LongueurChaine(s2); j++) {
            if (s1[i] != s2[j]) return i;
        }
    }return -1;
}

int findLastNotOf(char* s1, char* s2) {
    for (int i = LongueurChaine(s1) - 1; i >= 0; i--) {
        for (int j = 0; j < LongueurChaine(s2); j++) {
            if (s1[i] != s2[j]) return i;
        }
    }return -1;
}

char** split(char* s1, char* s2) {
    int i,j = 0;
    int lon;
    char* s3 = 'NULL';
    char* s4 = copieChaine(s1);
    char** t = (char*)calloc(20, sizeof(s1));
    while (findFirstOf(s4, s2) != -1) {
        lon = LongueurChaine(s4);
        i = findFirstOf(s4, s2);
        s3 = copieChaine(s4);
        s3[i] = '\0';
        *t[j++] = s3;
        s4 = copieChaine(s4 + i + 1);
    }return t;
}

char* toUpper(char* s) {
    int i,lon = LongueurChaine(s);
    char* s1 = (char*)calloc(lon, sizeof(char));
    for (i = 0; i < lon; i++) {
        if ('a' <= s[i] && s[i] <= 'z') s1[i] = s[i] - 'a' + 'A';
        else { s1[i] = s[i]; }
    }s1[i] = '\0';
    return s1;
}

char* inverseCase(char* s) {
    int i,lon = LongueurChaine(s);
    char* s1 = (char*)calloc(lon, sizeof(char));
    for (i = 0; i < lon; i++) {
        if ('a' <= s[i] && s[i] <= 'z') s1[i] = s[i] - 'a' + 'A';
        else if ('A' <= s[i] && s[i] <= 'Z') { s1[i] = s[i] + 'a' - 'A'; }
        else { s1[i] = s[i]; }
    }s1[i] = '\0';
    return s1;
}

