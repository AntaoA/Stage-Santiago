#include "mapfl.h"
#include "utils.h"

unsigned int* build_mapfl(unsigned int* LSA, int num_runs, int n, unsigned int* FT, int* SA) {
    unsigned int* mapFL = malloc(num_runs * sizeof(unsigned int));
    
    for (int p = 0; p < num_runs; p++) {
        int j = select1(FT, n, p);
        if (j == n-1) j = -1;
        for (int i = 0; i < n; i++) {
            if (SA[i] - 1 == j) {
                for (int q = 0; q < num_runs; q++) {
                    int u = i - 1;
                    if (u < 0) u = n - 1; 
                    unsigned int v = SA[u] - 1;
                    if (SA[u] ==  0) v = n - 1;
                    if (LSA[q] == v) {
                        mapFL[p] = q;
                        break;
                    }
                }
                break;
            }
        }
    }

    return mapFL;
}