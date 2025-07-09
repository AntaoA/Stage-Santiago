#include "phi.h"

int phi(int i, int n, int* SA, unsigned char* BWT) {
    int j = 0;
    for (int k = 1; k < n; k++) {
        if (SA[k] - 1 == i) {
            j = k;
            break;
        }
        else if (k == n - 1) {
            return -1; // If i is not found in SA, return -1
        }
    }
    if (BWT[j] != BWT[j - 1]) {
        return -1;
    } 
    if (SA[j-1] == 0) return n-1;
    return SA[j-1] - 1;
    
}