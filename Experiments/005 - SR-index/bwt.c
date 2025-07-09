#include "bwt.h"

int build_bwt(const unsigned char* T, int n, int* SA, unsigned char* BWT) {
    int primary_index = -1;

    for (int i = 0; i < n; i++) {
        if (SA[i] == 0) {
            BWT[i] = T[n - 1];         // wrap-around
            primary_index = i;         // position du suffixe vide
        } else {
            BWT[i] = T[SA[i] - 1];
        }
    }

    return primary_index;
}

unsigned int* start(BWT_Run* runs, int n, int num_runs) {
    unsigned int* starts = malloc(n * sizeof(unsigned int));
    int current_run = 0;
    for (int i = 0; i < n; i++) {
        if (current_run >= num_runs) {
           starts[i] = 0;
        }
        else if (i == runs[current_run].start) {
            starts[i] = 1;
            current_run++;
        } else {
            starts[i] = 0;
        }
    }
    return starts;
}

unsigned char* letter(BWT_Run* runs, int num_runs) {
    unsigned char* letters = malloc(sizeof(unsigned char) * num_runs);
    for (int i = 0; i < num_runs; i++) {
        letters[i] = runs[i].c;
    }
    return letters;
}