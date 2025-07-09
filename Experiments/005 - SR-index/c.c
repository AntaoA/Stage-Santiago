#include "c.h"
#include "utils.h"



unsigned int* build_C_from_text(const char* T, int n, const char* alphabet, int n_letter) {
    unsigned int* C = malloc(n_letter * sizeof(unsigned int));
    for (int i = 0; i < n_letter; i++) {
        C[i] = 0; // Initialisation à 0
    }

    for (int i = 0; i < n; i++) {
        unsigned char c = T[i];
        int index = letter_conv(c);
        if (index >= 0 && index < n_letter) {
            C[index]++;
        }
    }

    // Calcul des préfixes
    for (int i = 1; i < n_letter; i++) {
        C[i] += C[i - 1];
    }

    return C;
}