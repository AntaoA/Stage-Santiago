#include "findToehold.h"
#include "utils.h"

char findToehold(int iv, int pv, int n, int n_runs, int m,
                const unsigned int* FT, int* SA, const char* P,
                const unsigned char* letters, const unsigned int* starts,
                const unsigned int* del, const unsigned int* LF,
                const unsigned int* LSA) {
    if (iv == -1) {
        return SA[n] - m;
    }
    char c = P[iv];
    int q = select_c(letters, n_runs, c, rank_c(letters, pv, c));
    int j = select1(starts, n, q + 1) - 1;
    int k = 0;
    while ((j < n && starts[j+1] == 0) || del[q] == 1) {
        j = LF[j];
        q = rank1(starts, j);
        k++;
    }
    return LSA[rank0(del, q)] + k - (iv - 1);
}