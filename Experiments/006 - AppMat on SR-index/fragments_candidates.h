#ifndef ADE15490_9B5A_4C2C_9BDE_CF228C6C1D60
#define ADE15490_9B5A_4C2C_9BDE_CF228C6C1D60

#include "locate_approx.h"

typedef struct {
    int id; // Fragment ID
    char* text; // Fragment text
    approx_matching* matches; // Matches found in this fragment
} candidates;

candidates* create_candidates(srIndex* index, char* pattern, int k, int j);

void free_candidates(candidates* cand);

#endif /* ADE15490_9B5A_4C2C_9BDE_CF228C6C1D60 */
