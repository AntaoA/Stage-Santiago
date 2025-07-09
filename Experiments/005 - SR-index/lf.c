#include "lf.h"
#include "utils.h"

unsigned int* build_lf(unsigned int* C, unsigned char* BWT, int n) {
    unsigned int* LF = malloc(n * sizeof(unsigned int));
    for (int i = 0; i < n; i++) {
        unsigned char c = BWT[i];
        LF[i] = C[letter_conv(c) - 1] + rank_c(BWT, i, c);
    }
    return LF;
}