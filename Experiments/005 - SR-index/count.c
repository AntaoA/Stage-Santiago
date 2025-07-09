#include "count.h"
#include "utils.h"
#include <stdio.h>

plage count(const char *P, int n, int m, 
           const unsigned int *FT, int *SA,
           const unsigned char *letters, const unsigned int *starts,
           const unsigned int *del, const unsigned int *LF, const unsigned int *LSA,
           const unsigned int *C, const unsigned char *BWT) {
    int sp = 0;
    int ep = n-1;
    int iv = -1;
    int pv = -1;
    for (int i = m-1; i >= 0; i--) {
        char c = P[i];
        int p = rank1(starts, ep-1);
        if (c != letters[p-1]) {
            iv = i;
            pv = p;
        }
        sp = C[letter_conv(c) - 1] + rank_c(BWT, sp-1, c) + 1;
        ep = C[letter_conv(c) - 1] + rank_c(BWT, ep, c);
        if (sp > ep) {
            printf("No match found for the pattern.\n");
            return (plage){-1, -1, -1};
        }
    }
    int v = findToehold(iv, pv, n, n, m, FT, SA, P, letters, starts, del, LF, LSA);
    return (plage){sp-1, ep-1, v};
}