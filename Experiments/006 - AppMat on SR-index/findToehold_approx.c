#include "findToehold_approx.h"

int findToehold_approx(srIndex* index, int iv, int pv, char letter, int len_pattern) {
    if (iv == -1) {
        printf("SA[n] is stored\n");
        return index->last_sa - len_pattern;
    }
    printf("findToehold_approx: iv=%d, pv=%d, letter=%c\n", iv, pv, letter);
    int q = select_c(index->letters, index->n_runs, letter, rank_c(index->letters,  pv-1, letter)) + 1;
    printf("q=%d\n", q);
    int j = select1(index->start, index->n, q + 1);
    int k = 0;
    printf("Initial j=%d, k=%d\n", j, k);
    while ((j < index->n && index->start[j] == 0) || index->del[q-1] == 1) {
        j = index->LF[j];
        q = rank1(index->start, j) - 1;
        k++;
        printf("Updated j=%d, k=%d\n", j, k);
    }
    return index->lsa.LSA[rank0(index->del, q-1) - 1] + k - (iv - 1);
}