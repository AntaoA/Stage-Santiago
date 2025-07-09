#include "sr-index.h"

srIndex* create_sr_index(const char* text, int s) {
    int n = strlen(text);
    int n_letter = 0;
    const char* alphabet = "$ABDLR"; // Exemple d'alphabet
    n_letter = strlen(alphabet);

    unsigned char* input = malloc(n);
    memcpy(input, text, n);

    int* SA = malloc(n * sizeof(int));
    unsigned char* BWT = malloc(n * sizeof(unsigned char));

    build_suffix_array(input, n, SA);
    build_bwt(input, n, SA, BWT);

    int num_runs;
    BWT_Run* runs = extract_bwt_runs(BWT, n, &num_runs);

    unsigned int* starts = start(runs, n, num_runs);
    unsigned char* letters = letter(runs, num_runs);
    
    unsigned int* lsa = build_lsa(SA, runs, num_runs, n);
    
    unsigned int* del = malloc(num_runs * sizeof(unsigned int));
    for (int i = 0; i < num_runs; i++) {
        del[i] = (i % s == 0) ? 1 : 0;
    }

    LSA s_lsa = build_subsample_lsa(lsa, num_runs, s, del);

    unsigned int* FT = build_ft(SA, BWT, n);
    unsigned int* s_FT = build_s_ft(SA, BWT, n, del);
    
    unsigned int* mapFL = build_mapfl(lsa, num_runs, n, FT, SA);
    unsigned int* s_mapFL = build_mapfl(s_lsa.LSA, s_lsa.len, n, s_FT, SA);
    
    unsigned int* C = build_C_from_text(text, n, alphabet, n_letter);
    
    unsigned int* LF = build_lf(C, BWT, n);

    srIndex* index = malloc(sizeof(srIndex));
    index->s = s;
    index->alphabet = alphabet;
    index->n_letter = n_letter;
    index->n = n;
    index->start = starts;
    index->letters = letters;
    index->lsa.LSA = s_lsa.LSA;
    index->lsa.len = s_lsa.len;
    index->del = del;
    index->FT = s_FT;
    index->mapFL = s_mapFL;
    index->C = C;
    index->LF = LF;
    free(input);
    free(SA);
    free(BWT);
    free(runs);
    free(lsa);
    free(FT);
    free(mapFL);
    return index;
}