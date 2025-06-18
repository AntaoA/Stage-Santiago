#define _GNU_SOURCE
#include "fragmentsAM.h"
#include "rabin_karp.h"





FragmentsList* create_fragment_list() {
    FragmentsList* list = malloc(sizeof(FragmentsList));
    list->size = 0;
    list->capacity = 8;
    list->fragment = malloc(sizeof(Fragment) * list->capacity);
    return list;
}

CandidatesList* create_candidate_list() {
    CandidatesList* list = malloc(sizeof(CandidatesList));
    list->size = 0;
    list->capacity = 8;
    list->candidate = malloc(sizeof(Candidate) * list->capacity);
    return list;
}

Candidate copy_candidate(Candidate* src) {
    Candidate dest;
    dest.fragment.start = src->fragment.start;
    dest.fragment.end = src->fragment.end;
    dest.fragment.id = src->fragment.id;
    dest.fragment.text = strdup(src->fragment.text);
    dest.start_text = src->start_text;
    dest.end_text = src->end_text;
    return dest;
}

void free_alignment(Alignment* alignment) {
    free(alignment->operations);
    free(alignment);
}

void free_fragment_list(FragmentsList* list) {
    for (int i = 0; i < list->size; i++)
        free(list->fragment[i].text);
    free(list->fragment);
    free(list);
}

void free_candidate_list(CandidatesList* list) {
    for (int i = 0; i < list->size; i++)
        free(list->candidate[i].fragment.text);
    free(list->candidate);
    free(list);
}

void print_fragment_list(FragmentsList* list) {
    for (int i = 0; i < list->size; i++)
        printf("Fragment %d: '%s' from [%d, %d]\n",
               list->fragment[i].id, list->fragment[i].text,
               list->fragment[i].start, list->fragment[i].end);
}

void print_candidate_list(CandidatesList* list) {
    for (int i = 0; i < list->size; i++)
        printf("Candidate %d: Fragment '%s' (%d) at text position [%d, %d]\n",
               i, list->candidate[i].fragment.text, list->candidate[i].fragment.id,
               list->candidate[i].start_text, list->candidate[i].end_text);
}

void strrev(char *str) {
    int i = 0, j = strlen(str) - 1;
    while (i < j) {
        char tmp = str[i];
        str[i] = str[j];
        str[j] = tmp;
        i++; j--;
    }
}

// === Fragment Splitting ===
FragmentsList* make_fragments(char* text, int k) {
    FragmentsList* list = create_fragment_list();
    int len = strlen(text);
    int len_frag = len / (k + 1);
    int nb_frag_high = len % (k + 1);
    int start = 0;

    for (int i = 0; i < k + 1; i++) {
        Fragment frag;
        frag.start = start;
        frag.id = i;

        int frag_len = (i < nb_frag_high) ? len_frag + 1 : len_frag;
        frag.end = frag.start + frag_len - 1;
        frag.text = malloc(sizeof(char) * (frag_len + 1));
        strncpy(frag.text, text + frag.start, frag_len);
        frag.text[frag_len] = '\0';

        start += frag_len;

        if (list->size >= list->capacity) {
            list->capacity *= 2;
            list->fragment = realloc(list->fragment, sizeof(Fragment) * list->capacity);
        }

        list->fragment[list->size++] = frag;
    }

    return list;
}

CandidatesList* exact_match(char* text, Fragment frag) {
    CandidatesList* list = create_candidate_list();
    rabin_karp(text, frag, list);
    return list;
}

void add_candidates(CandidatesList* dest, CandidatesList* src) {
    for (int i = 0; i < src->size; i++) {
        if (dest->size >= dest->capacity) {
            dest->capacity *= 2;
            dest->candidate = realloc(dest->candidate, sizeof(Candidate) * dest->capacity);
        }
        dest->candidate[dest->size++] = copy_candidate(&src->candidate[i]);
    }
}

CandidatesList* find_candidates(char* text, char* pattern, int k) {
    FragmentsList* fragments = make_fragments(pattern, k);
    print_fragment_list(fragments);
    CandidatesList* all_candidates = create_candidate_list();

    for (int i = 0; i < fragments->size; i++) {
        CandidatesList* matches = exact_match(text, fragments->fragment[i]);
        add_candidates(all_candidates, matches);
        free_candidate_list(matches);
    }

    free_fragment_list(fragments);
    return all_candidates;
}


// === Edit Distance Computation ===
Alignment* edit_distance_aligned(char* text, char* pattern) {
    Alignment* alignment = malloc(sizeof(Alignment));
    int m = strlen(pattern);
    int n = strlen(text);

    int* dp = malloc((m + 1) * (n + 1) * sizeof(int));
    #define DP(i,j) dp[(i)*(n+1)+(j)]
    DP(0,0) = 0;

    Operation* back = malloc((m + 1) * (n + 1) * sizeof(Operation));
    #define BACK(i,j) back[(i)*(n+1)+(j)]
    BACK(0,0) = E;

    for (int j = 1; j <= n; j++) {
        DP(0,j) = INF;
        BACK(0,j) = I;
    }
    for (int i = 1; i <= m; i++) {
        DP(i,0) = i;
        BACK(i,0) = D;
    }

    for (int i = 1; i <= m; i++) {
        for (int j = 1; j <= n; j++) {
            int cost_sub = (pattern[i - 1] == text[j - 1]) ? 0 : 1;
            int del = DP(i-1,j) + 1;
            int ins = DP(i,j-1) + 1;
            int sub = DP(i-1,j-1) + cost_sub;

            DP(i,j) = sub;
            BACK(i,j) = cost_sub ? S : E;

            if (del < DP(i,j)) { DP(i,j) = del; BACK(i,j) = D; }
            if (ins < DP(i,j)) { DP(i,j) = ins; BACK(i,j) = I; }
        }
    }

    int min_k = INF, end_j = -1;
    for (int j = 0; j <= n; j++) {
        if (DP(m,j) < min_k) {
            min_k = DP(m,j);
            end_j = j;
        }
    }

    Operation* ops = malloc(sizeof(Operation) * (m + n));
    int nbOp = 0;
    int i = m, j = end_j;
    while (i > 0 || j > 0) {
        Operation op = BACK(i,j);
        ops[nbOp++] = op;
        if (op == E || op == S) { i--; j--; }
        else if (op == D) { i--; }
        else if (op == I) { j--; }
    }

    for (int a = 0; a < nbOp / 2; a++) {
        Operation tmp = ops[a];
        ops[a] = ops[nbOp - 1 - a];
        ops[nbOp - 1 - a] = tmp;
    }

    free(dp);
    free(back);
    alignment->k = min_k;
    alignment->nbOp = nbOp;
    alignment->operations = ops;
    return alignment;
}

// === Append Utility for Debug Strings ===
void append_char(char* s, char c) {
    size_t len = strlen(s);
    s[len] = c;
    s[len + 1] = '\0';
}

// === Reverse Operations in Alignment ===
void revOperations(Alignment* align) {
    for (int i = 0; i < align->nbOp / 2; i++) {
        Operation tmp = align->operations[i];
        align->operations[i] = align->operations[align->nbOp - 1 - i];
        align->operations[align->nbOp - 1 - i] = tmp;
    }
}

// === Get Start Position from Alignment ===
int getStartPositionAlignment(Alignment* align, int endPosition) {
    int start = endPosition + 1; // Start after the last character of the match
    for (int i = align->nbOp - 1; i >= 0; i--) {
        switch (align->operations[i]) {
            case D: break;
            default: // I, S, E
                start--;
                break;
        }
    }
    return start;
}

// === Candidate Verification by Approximate Matching ===
void verify_candidates(CandidatesList* candidates_list, char* text, char* pattern, int k) {
    int text_len = strlen(text);
    int pattern_len = strlen(pattern);

    for (int i = 0; i < candidates_list->size; i++) {
        Candidate* cand = &candidates_list->candidate[i];
        int start_left = MAX(cand->start_text - cand->fragment.start - k, 0);

        char text_left[cand->start_text - start_left + 1];
        strncpy(text_left, text + start_left, cand->start_text - start_left);
        text_left[cand->start_text - start_left] = '\0';

        char pattern_left[cand->fragment.start + 1];
        strncpy(pattern_left, pattern, cand->fragment.start);
        pattern_left[cand->fragment.start] = '\0';

        strrev(text_left);
        strrev(pattern_left);
        Alignment* align_left = edit_distance_aligned(text_left, pattern_left);
        revOperations(align_left);
        int k_left = align_left->k;
        if (k_left > k) continue;

        int k_right_max = k - k_left;
        int end_right = MIN(cand->end_text + (pattern_len - cand->fragment.end - 1) + k_right_max, text_len);

        char text_right[end_right - cand->end_text + 1];
        strncpy(text_right, text + cand->end_text + 1, end_right - cand->end_text);
        text_right[end_right - cand->end_text] = '\0';

        char pattern_right[pattern_len - cand->fragment.end];
        strncpy(pattern_right, pattern + cand->fragment.end + 1, pattern_len - cand->fragment.end);
        pattern_right[pattern_len - cand->fragment.end - 1] = '\0';

        Alignment* align_right = edit_distance_aligned(text_right, pattern_right);
        int k_right = align_right->k;

        if (k_right > k_right_max) continue;

        printf("Candidate %d is valid with %d errors.\n", i, k_left + k_right);
        char t[100] = "", p[100] = "", op[100] = "";
        int i_p = 0;
        int i_t = getStartPositionAlignment(align_left, cand->start_text - 1);

        for (int j = 0; j < align_left->nbOp; j++) {
            switch (align_left->operations[j]) {
                case D: append_char(t, '-'); append_char(p, pattern[i_p++]); append_char(op, ' '); break;
                case I: append_char(t, text[i_t++]); append_char(p, '-'); append_char(op, ' '); break;
                case S: append_char(t, text[i_t++]); append_char(p, pattern[i_p++]); append_char(op, '*'); break;
                case E:  append_char(t, text[i_t++]); append_char(p, pattern[i_p++]); append_char(op, '|'); break;
            }
        }

        for (int j = 0; j < cand->fragment.end - cand->fragment.start; j++) {
            append_char(t, text[i_t]);
            append_char(p, pattern[i_p]);
            append_char(op, '|');
            i_t++; i_p++;
        }

        for (int j = 0; j < align_right->nbOp; j++) {
            switch (align_right->operations[j]) {
                case D: append_char(t, '-'); append_char(p, pattern[i_p++]); append_char(op, ' '); break;
                case I: append_char(t, text[i_t++]); append_char(p, '-'); append_char(op, ' '); break;
                case S: append_char(t, text[i_t++]); append_char(p, pattern[i_p++]); append_char(op, '*'); break;
                case E:  append_char(t, text[i_t++]); append_char(p, pattern[i_p++]); append_char(op, '|'); break;
            }
        }

        printf("%s\n%s\n%s\n", t, op, p);
    }
}