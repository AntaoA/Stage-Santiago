#include "ft.h"

unsigned int* build_ft(int* SA, unsigned char* BWT, int n) {
    unsigned int* FT = calloc(n, sizeof(unsigned int)); // initialise à 0
    int j_0 = SA[0] - 1;
    if (j_0 < 0) j_0 = n - 1;
    FT[j_0] = 1;
    for (int i = 1; i < n; i++) {
        if (BWT[i] != BWT[i-1]) {
            int j = SA[i] - 1;
            if (j < 0) j = n - 1;
            FT[j] = 1;
        }
    }
    return FT;
}


unsigned int* build_s_ft(int* SA, unsigned char* BWT, int n, unsigned int* del) {
    unsigned int* s_FT = calloc(n, sizeof(unsigned int)); // initialise à 0
    int j_0 = SA[0] - 1;
    if (j_0 < 0) j_0 = n - 1;
    s_FT[j_0] = 1;
    int current_run = 0;
    for (int i = 1; i < n; i++) {
        if (BWT[i] != BWT[i-1]) {
            current_run++;
            if (del[current_run] == 0) {
                int j = SA[i] - 1;
                if (j < 0) j = n - 1;
                s_FT[j] = 1;
            }
        }
    }
    return s_FT;
}