#include "locate.h"


int checkSample (int em, int sp, int k, unsigned int* starts,
                unsigned int* del, unsigned int* LSA, unsigned int* V) {
    int p = rank1(starts, em);
    if (del[p] == 1) {
        return em;
    }
    V[em-sp+1] = LSA[rank0(del, p)] + 1 + k;
    return em-1;
}




void locateRec (int sm, int em, int k, int n, int s, int sp,
                unsigned int* V, unsigned int* starts,
                unsigned int* del, unsigned int* LSA,
                unsigned int* LF, unsigned int* FT, unsigned int* mapFL) {
    if (k == s) {
        for (int i = em; i >= sm; i--) {
            V[i-sp+1] = phi(V[i-sp+2], n, FT, LSA, mapFL);
        }
        return;
    }
    int p = rank1(starts, em);
    int q = rank1(starts, sm);
    for (int j = p; j >= q; j--) {
        if ((j < p) || starts[em+1] == 1) {
            em = checkSample(em, sp, k, starts, del, LSA, V);
        }
        int im = max(select1(starts, n, j), sm);
        if (im > em) {
            continue;
        }
        locateRec(LF[im], LF[em], k+1, n, s, sp, V, starts, del, LSA, LF, FT, mapFL);
        em = im - 1;
    }
    return;
}

unsigned int* locate (int n, int s, char* P, unsigned int* starts,
                      unsigned int* del, unsigned char* letters, unsigned int* LSA,
                      unsigned int* FT, unsigned int* C, unsigned char* BWT,
                      int* SA, unsigned int* LF, unsigned int* mapFL) {

    unsigned int* V = malloc((n) * sizeof(unsigned int));
    plage res = count(P, n, strlen(P), FT, SA, letters, starts, del, LF, LSA, C, BWT);
    int sp = res.sp;
    int ep = res.ep;
    int v = res.v;
    V[ep - sp + 1] = v;
    if (sp < ep) {
        locateRec(sp, ep - 1, 0, n, s, sp, V, starts, del, LSA, LF, LF, mapFL);
    }
    return V;
}
