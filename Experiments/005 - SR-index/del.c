#include "del.h"

int* build_del(int n) {
    int* del = malloc(n * sizeof(int));
    for (int i = 0; i < n; i++) {
        del[i] = 0; // Initialisation à -1
    }

    return del;
}