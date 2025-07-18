#include "approx_forward.h"












typedef struct {
    int pos;
    char letter;
} forward_t;

forward_t next_letter(srIndex* index, unsigned int pos) {
    if (pos <= index->C[0]) {
        int n_c = pos;
        int p = select_c_bwt('$', n_c, index->start, index->letters);
        return (forward_t){p+1, '$'};
    }
    else if (pos <= index->C[1]) {
        int n_c = pos - index->C[0];
        int p = select_c_bwt('A', n_c, index->start, index->letters);
        return (forward_t){p+1, 'A'};
    }
    else if (pos <= index->C[2]) {
        int n_c = pos - index->C[1];
        int p = select_c_bwt('B', n_c, index->start, index->letters);
        return (forward_t){p+1, 'B'};
    }
    else if (pos <= index->C[3]) {
        int n_c = pos - index->C[2];
        int p = select_c_bwt('D', n_c, index->start, index->letters);
        return (forward_t){p+1, 'D'};
    }
    else if (pos <= index->C[4]) {
        int n_c = pos - index->C[3];
        int p = select_c_bwt('L', n_c, index->start, index->letters);
        return (forward_t){p+1, 'L'};
    }
    else {
        int n_c = pos - index->C[4];
        int p = select_c_bwt('R', n_c, index->start, index->letters);
        return (forward_t){p+1, 'R'};
    }
}

int count_forward_errors(srIndex* index, const char* pattern, int k, int position_sa, int position_pattern) {
    int m = strlen(pattern);

    if (k < 0) {
        return -1;
    }

    if (position_pattern == m) {
        return k;
    }
    
    char c = pattern[position_pattern];
    forward_t next = next_letter(index, position_sa);

    
    int err_match_sub;
    
    if (next.letter == c) {
        // match
        err_match_sub = count_forward_errors(index, pattern, k, next.pos, position_pattern + 1);
    } else {
        // substitution
        err_match_sub = count_forward_errors(index, pattern, k - 1, next.pos, position_pattern + 1);
    }

    int err_ins = count_forward_errors(index, pattern, k - 1, next.pos, position_pattern);
    int err_del = count_forward_errors(index, pattern, k - 1, position_sa, position_pattern + 1);

    int max_err = -1;
    if (err_match_sub >= 0) {
        max_err = err_match_sub;
    }
    if (err_ins >= 0 && (err_ins > max_err)) {
        max_err = err_ins;
    }
    if (err_del >= 0 && (err_del > max_err)) {
        max_err = err_del;
    }
    return max_err;
}

int approx_forward(srIndex* index, const char* pattern, int k, int position_sa, int nb_seen) {

    unsigned int pos_sa = (unsigned int)position_sa;
    printf("nb_seen: %d\n", nb_seen);

    for (int i = 0; i < nb_seen; i++) {
        forward_t next = next_letter(index, pos_sa);
        pos_sa = next.pos;
    }

    int err_remain = count_forward_errors(index, pattern, k, pos_sa, 0);

    return err_remain;
}
