#include "verify.h"
#include "approx_forward.h"

verification_result verify_left(srIndex* index, int position, int position_sa, int k_left_max, const char* pattern, int id_left) {

    if (id_left == 0) {
        return (verification_result){.err_max = k_left_max, .position_start = position};
    }

    char* p = (char*)malloc((id_left + 1) * sizeof(char));
    for (int i = 0; i < id_left; i++) {
        p[i] = pattern[i];
    }
    p[id_left] = '\0';

    printf("Pattern: %s, max_errors: %d\n", p, k_left_max);

    intervalle_list* result = count_approx(index, p, k_left_max, position_sa, position_sa);

    if (result->size == 0) {
        printf("No matches found.\n");
        return (verification_result){.err_max = -1, .position_start = -1};
    }

    printf("Matches found:\n");
    for (int i = 0; i < result->size; i++) {
        printf("Interval %d: [%d, %d], Errors: %d, Start in text: %d\n", i, result->intervals[i].sp, result->intervals[i].ep, result->intervals[i].errors, position - result->nb_seen[i]);
    }
    printf("\n");


    int pos = result->nb_seen[0];
    int k = result->intervals[0].errors;

    for (int i = 1; i < result->size; i++) {
        if (result->intervals[i].errors < k) {
            pos = result->nb_seen[i];
            k = result->intervals[i].errors;
        }
    }

    return (verification_result){.err_max = k_left_max - k, .position_start = pos};
}

int verify_right(srIndex *index, int position, int position_sa, int k_right_max, const char *pattern, int id_right, int nb_letters_seen_in_text) {
    int len_pattern = strlen(pattern);
    

    if (id_right == len_pattern) {
        return k_right_max;
    }

    char* p = (char*)malloc((len_pattern - id_right) * sizeof(char));
    for (int i = 0; i < len_pattern - id_right; i++) {
        p[i] = pattern[id_right + i];
    }
    p[len_pattern - id_right] = '\0';

    printf("Pattern: %s, max_errors: %d\n", p, k_right_max);



    int err_remain = approx_forward(index, p, k_right_max, position_sa, nb_letters_seen_in_text);
    if (err_remain < 0) {
        return -1;
    }
    return k_right_max - err_remain;
}