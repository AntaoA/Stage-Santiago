#include "lsa.h"


BWT_Run* extract_bwt_runs(unsigned char* BWT, int n, int* num_runs) {
    BWT_Run* runs = malloc(n * sizeof(BWT_Run)); // au pire 1 run par caractère
    int r = 0;

    int start = 0;
    for (int i = 1; i < n; i++) {
        if (BWT[i] != BWT[i - 1]) {
            runs[r++] = (BWT_Run){ .c = BWT[i - 1], .start = start, .end = i - 1 };
            start = i;
        }
    }
    // dernier run
    runs[r++] = (BWT_Run){ .c = BWT[n - 1], .start = start, .end = n - 1 };

    *num_runs = r;
    return runs;
}

unsigned int* build_lsa(int* SA, BWT_Run* runs, int num_runs, int n) {
    unsigned int* LSA = malloc(num_runs * sizeof(unsigned int));

    for (int p = 0; p < num_runs; p++) {
        int i = runs[p].end;  // indice dans la BWT
        LSA[p] = (SA[i] - 1 + n) % n;
    }

    return LSA;
}
static int compare_by_value(const void* a, const void* b) {
    return ((LSAEntry*)a)->lsa_value - ((LSAEntry*)b)->lsa_value;
}

// Subsample LSA canonique : garde des positions à distance >= s
LSA build_subsample_lsa(unsigned int* lsa, int num_runs, int s, unsigned int* del) {
    LSAEntry* entries = malloc(num_runs * sizeof(LSAEntry));
    for (int i = 0; i < num_runs; i++) {
        entries[i].lsa_value = lsa[i];
        entries[i].p = i;
        del[i] = 0;  // initialisation
    }

    // Tri croissant par valeur de LSA[p]
    qsort(entries, num_runs, sizeof(LSAEntry), compare_by_value);

    // Glouton canonique
    int nb_kept = num_runs;
    int last_kept_pos = -s - 1;
    for (int i = 0; i < num_runs; i++) {
        if (entries[i].lsa_value - last_kept_pos < s) {
            del[entries[i].p] = 1;
            nb_kept--;
        }
        else {
            last_kept_pos = entries[i].lsa_value;
        }
    }

    unsigned int* s_lsa = malloc(nb_kept * sizeof(unsigned int));
    int j = 0;
    for (int i = 0; i < num_runs; i++) {
        if (!del[i]) {
            s_lsa[j] = lsa[i];
            j++;
        }
    }
    free(entries);
    return (LSA){ .LSA = s_lsa, .len = nb_kept };
}