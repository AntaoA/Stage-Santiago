#include "utils.h"


int rank1(const unsigned int* FT, int n) {
    int count = 0;
    for (int i = 0; i <= n; i++) {
        if (FT[i] == 1) {
            count++;
        }
    }
    return count;
}

int select1(const unsigned int* FT, int n, int rank) {

    if (rank == 0) return 0;
    
    int count = 0;
    for (int i = 0; i < n; i++) {
        if (FT[i] == 1) {
            if (count == rank) {
                return i; // Retourne la position du rang demandé
            }
            count++;
        }
    }
    return -1; // Rang non trouvé
}

int select_c(const unsigned char* FT, int len, unsigned char c, int n_c) {
    int count = 0;
    for (int i = 0; i < len; i++) {
        if (FT[i] == c) {
            count++;
            if (count == n_c) {
                return i; // Retourne la position du n-ème caractère c
            }
        }
    }
    return -1; // n-ème caractère c non trouvé
}

int pred1(const unsigned int* FT, int n, int pos) {
    if (pos <= 0 || pos >= n) {
        return -1; // Position invalide
    }
    for (int i = pos - 1; i >= 0; i--) {
        if (FT[i] == 1) {
            return i; // Retourne la position du prédécesseur
        }
    }
    return -1; // Aucun prédécesseur trouvé
}

int rank0(const unsigned int *FT, int n) {

    int count = 0;
    for (int i = 0; i <= n; i++) {
        if (FT[i] == 0) {
            count++;
        }
    }
    return count;
}

int rank_c(const unsigned char* FT, int n, unsigned char c) {
    int count = 0;
    for (int i = 0; i < n; i++) {
        if (FT[i] == c) {
            count++;
        }
    }
    return count; // Retourne le nombre d'occurrences du caractère c
}

int letter_conv(unsigned char c) {
    if (c == 'A') return 1;
    if (c == 'B') return 2;
    if (c == 'D') return 3;
    if (c == 'L') return 4;
    if (c == 'R') return 5;
    if (c == '$') return 0;
    return -1; // Caractère non reconnu
}

int max(unsigned int a, unsigned int b) {
    return (a > b) ? a : b;
}