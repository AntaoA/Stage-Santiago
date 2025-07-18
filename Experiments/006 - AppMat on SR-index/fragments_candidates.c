#include "fragments_candidates.h"

candidates* create_candidates(srIndex* index, char* pattern, int k, int j) {
    int m = strlen(pattern);
    int len_frag = m / (j + 1);
    int nb_frag_high = m % (j + 1);
    int start = 0;

    candidates* cand = (candidates*)malloc(sizeof(candidates) * (j + 1));

    for (int i = 0; i < j + 1; i++) {
        cand[i].id = i;
        int frag_len = (i < nb_frag_high) ? len_frag + 1 : len_frag;
        cand[i].text = (char*)malloc(sizeof(char) * (frag_len + 1));
        strncpy(cand[i].text, pattern + start, frag_len);
        cand[i].text[frag_len] = '\0';

        // Locate matches in the fragment
        approx_matching* matches = locate_approx(index, cand[i].text, k);
        cand[i].matches = matches;

        start += frag_len;
    }
    return cand;
}

void free_candidates(candidates* cand) {
    for (int i = 0; i < cand->id; i++) {
        free(cand[i].text);
        free(cand[i].matches->matches);
        free(cand[i].matches);
    }
    free(cand);
}