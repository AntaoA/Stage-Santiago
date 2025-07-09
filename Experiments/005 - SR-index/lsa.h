#ifndef C60A8FDD_B6FE_437C_91E2_817E3641C91C
#define C60A8FDD_B6FE_437C_91E2_817E3641C91C
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

typedef struct {
    unsigned char c;  // lettre du run
    int start;        // position de début dans la BWT
    int end;          // position de fin (inclus)
} BWT_Run;

typedef struct {
    int lsa_value;  // valeur LSA[p]
    int p;          // index du run dans LSA[]
} LSAEntry;

typedef struct {
    unsigned int* LSA;
    int len;
} LSA;

BWT_Run* extract_bwt_runs(unsigned char* BWT, int n, int* num_runs);

unsigned int* build_lsa(int* SA, BWT_Run* runs, int num_runs, int n);

LSA build_subsample_lsa(unsigned int* LSA, int num_runs, int s, unsigned int* keep);

#endif /* C60A8FDD_B6FE_437C_91E2_817E3641C91C */
