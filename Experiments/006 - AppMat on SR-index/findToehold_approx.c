#include "findToehold_approx.h"

int findToehold_approx(srIndex* index, char* pattern, int iv, int pv) {
    if (iv == -1) return -1;
    char c = pattern[iv];
    int q = select_c(index->letters, index->lsa.len, c, rank_c(index->letters, pv, c));
    int j = select1(index->start, index->n, q + 1) - 1;
    int k = 0;
    while ((j < index->n && index->start[j + 1] == 0) || index->del[q] == 1) {
        j = index->LF[j];
        q = rank1(index->start, j);
        k++;
    }
    return index->lsa.LSA[rank0(index->del, q)] + k - (iv - 1);
}