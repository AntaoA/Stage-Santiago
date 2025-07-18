#include "locate_approx.h"



int phi(int i) {
    if (i == 1) return 7;
    if (i == 2) return 8;
    if (i == 3) return 9;
    if (i == 4) return 14;
    if (i == 5) return 11;
    if (i == 6) return 12;
    if (i == 7) return 13;
    if (i == 8) return 16;
    if (i == 9) return 17;
    if (i == 10) return 1;
    if (i == 11) return 2;
    if (i == 12) return 3;
    if (i == 13) return 4;
    if (i == 14) return 5;
    if (i == 15) return 6;
    if (i == 16) return 13;
    if (i == 17) return 18;
    if (i == 18) return -1;
    return -1; // Invalid ùinput
}


void locate(srIndex* index, int sm_i, int em_i, char* pattern, int k, int* locations, int sp) {
    printf("locate: sm=%d, em=%d, pattern='%s', k=%d, sp=%d\n", sm_i, em_i, pattern, k, sp);
    int sm = sm_i;
    int em = em_i;
    int n = index->n;
    if (k == index->s) {
        printf("Reached maximum errors, returning.\n");
        for (int im = em; im >= sm; im--) {
            int i = locations[im - sp + 1];
            locations[im - sp] = phi(i);
        }
    }
    else {
        if (index->start[em] == 1) {
            printf("End of segment reached at em=%d, returning.\n", em);
            int q = rank1(index->start, em - 1);
            if (index->del[q-1] == 0) {
                locations[em - sp] = index->lsa.LSA[rank0(index->del, q)] + 1 + k;
                em--;
            }
        }
        int q = rank1(index->start, sm);
        while (sm <= em) {
            printf("Processing segment: sm=%d, em=%d, q=%d\n", sm, em, q);
            int im;
            if (q == index->n_runs) {
                im = em + 1;
            } else {
                im = select1(index->start, index->n, q + 1) + 1;
            }
            
            if (im - 1 > em) im = em + 1;
            locate(index, sm, im - 1, pattern, k + 1, locations, sp);
            sm = im;
            q++;
        }
    }
}


approx_matching* locate_approx(srIndex* index, char* pattern, int k) {

    intervalle_list* result = count_approx(index, pattern, k, 1, index->n);

    for (int i = 0; i < result->size; i++) {
        printf("Interval %d: [%d, %d], Errors: %d, Toehold: %d\n", i, result->intervals[i].sp, result->intervals[i].ep, result->intervals[i].errors, result->toeholds[i]);
    }

    if (result == NULL) {
        approx_matching* empty_result = (approx_matching*)malloc(sizeof(approx_matching));
        empty_result->pattern = pattern;
        empty_result->max_errors = k;
        empty_result->match_count = 0;
        empty_result->matches = NULL;
        
        return empty_result;
    }

    int nb_matches = 0;

    for (int i = 0; i < result->size; i++) {
        nb_matches += result->intervals[i].ep - result->intervals[i].sp + 1;
    }

    approx_matching* appmat = (approx_matching*)malloc(sizeof(approx_matching));
    appmat->pattern = pattern;
    appmat->max_errors = k;
    appmat->match_count = nb_matches;
    appmat->matches = (match*)malloc(nb_matches * sizeof(match));

    int idx = 0;

    for (int i = 0; i < result->size; i++) {
        
        
        int * locations = (int*)malloc((result->intervals[i].ep - result->intervals[i].sp + 1) * sizeof(int));
        locations[result->intervals[i].ep - result->intervals[i].sp] = result->toeholds[i];
        if (result->intervals[i].sp < result->intervals[i].ep) {
            locate(index, result->intervals[i].sp, result->intervals[i].ep - 1, pattern, 0, locations, result->intervals[i].sp);
        }
        printf("Locations for interval %d: ", i);
        for (int j = 0; j < result->intervals[i].ep - result->intervals[i].sp + 1; j++)     {
            printf("%d ", locations[j]);
            appmat->matches[idx++] = (match){locations[j], result->intervals[i].errors, .position_sa = j + result->intervals[i].sp, .nb_letters_seen_in_text = result->nb_seen[i]};
        }
        printf("\n");
    }

    return appmat;
}