#define _GNU_SOURCE
#include "fragmentsAM.h"
#include "rabin_karp.h"




// === Fragment & Candidate Management ===
Alignment create_alignment() {
    return (Alignment){ .k = 0, .nbOp = 0, .operations = NULL };
}

FragmentList* create_fragment_list() {
    FragmentList* list = malloc(sizeof(FragmentList));
    list->size = 0;
    list->capacity = 8;
    list->fragments = malloc(sizeof(Fragment) * list->capacity);
    return list;
}

CandidateList* create_candidate_list() {
    CandidateList* list = malloc(sizeof(CandidateList));
    list->size = 0;
    list->capacity = 8;
    list->candidates = malloc(sizeof(Candidate) * list->capacity);
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

void free_fragment_list(FragmentList* list) {
    for (int i = 0; i < list->size; i++)
        free(list->fragments[i].text);
    free(list->fragments);
    free(list);
}

void free_candidate_list(CandidateList* list) {
    for (int i = 0; i < list->size; i++)
        free(list->candidates[i].fragment.text);
    free(list->candidates);
    free(list);
}

void print_fragment_list(FragmentList* list) {
    for (int i = 0; i < list->size; i++)
        printf("Fragment %d: '%s' from [%d, %d]\n",
               list->fragments[i].id, list->fragments[i].text,
               list->fragments[i].start, list->fragments[i].end);
}

void print_candidate_list(CandidateList* list) {
    for (int i = 0; i < list->size; i++)
        printf("Candidate %d: Fragment '%s' (%d) at text position [%d, %d]\n",
               i, list->candidates[i].fragment.text, list->candidates[i].fragment.id,
               list->candidates[i].start_text, list->candidates[i].end_text);
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
FragmentList* make_fragments(char* text, int k) {
    FragmentList* list = create_fragment_list();
    int len = strlen(text);
    int len_frag = len / (k + 1);
    int nb_frag_high = len % (k + 1);
    int start = 0;

    for (int i = 0; i < k + 1; i++) {
        Fragment frag;
        frag.start = start;
        frag.id = i;

        int frag_len = (i < nb_frag_high) ? len_frag + 1 : len_frag;
        frag.end = frag.start + frag_len;
        frag.text = malloc(sizeof(char) * (frag_len + 1));
        strncpy(frag.text, text + frag.start, frag_len);
        frag.text[frag_len] = '\0';

        start += frag_len;

        if (list->size >= list->capacity) {
            list->capacity *= 2;
            list->fragments = realloc(list->fragments, sizeof(Fragment) * list->capacity);
        }

        list->fragments[list->size++] = frag;
    }

    return list;
}

CandidateList* exact_match(char* text, Fragment frag) {
    CandidateList* list = create_candidate_list();
    rabin_karp(text, frag, list);
    return list;
}

void add_candidates(CandidateList* dest, CandidateList* src) {
    for (int i = 0; i < src->size; i++) {
        if (dest->size >= dest->capacity) {
            dest->capacity *= 2;
            dest->candidates = realloc(dest->candidates, sizeof(Candidate) * dest->capacity);
        }
        dest->candidates[dest->size++] = copy_candidate(&src->candidates[i]);
    }
}

CandidateList* find_candidates(char* text, char* pattern, int k) {
    FragmentList* fragments = make_fragments(pattern, k);
    print_fragment_list(fragments);
    CandidateList* all_candidates = create_candidate_list();

    for (int i = 0; i < fragments->size; i++) {
        CandidateList* matches = exact_match(text, fragments->fragments[i]);
        add_candidates(all_candidates, matches);
        free_candidate_list(matches);
    }

    free_fragment_list(fragments);
    return all_candidates;
}


// === Edit Distance Computation ===
Alignment edit_distance_aligned(char* text, char* pattern) {
    int m = strlen(pattern);
    int n = strlen(text);

    int* dp = malloc((m + 1) * (n + 1) * sizeof(int));
    #define DP(i,j) dp[(i)*(n+1)+(j)]
    DP(0,0) = 0;

    Operation* back = malloc((m + 1) * (n + 1) * sizeof(Operation));
    #define BACK(i,j) back[(i)*(n+1)+(j)]
    BACK(0,0) = EQ;

    for (int j = 1; j <= n; j++) {
        DP(0,j) = INF;
        BACK(0,j) = INS;
    }
    for (int i = 1; i <= m; i++) {
        DP(i,0) = i;
        BACK(i,0) = DEL;
    }

    for (int i = 1; i <= m; i++) {
        for (int j = 1; j <= n; j++) {
            int cost_sub = (pattern[i - 1] == text[j - 1]) ? 0 : 1;
            int del = DP(i-1,j) + 1;
            int ins = DP(i,j-1) + 1;
            int sub = DP(i-1,j-1) + cost_sub;

            DP(i,j) = sub;
            BACK(i,j) = cost_sub ? SUB : EQ;

            if (del < DP(i,j)) { DP(i,j) = del; BACK(i,j) = DEL; }
            if (ins < DP(i,j)) { DP(i,j) = ins; BACK(i,j) = INS; }
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
        if (op == EQ || op == SUB) { i--; j--; }
        else if (op == DEL) { i--; }
        else if (op == INS) { j--; }
    }

    for (int a = 0; a < nbOp / 2; a++) {
        Operation tmp = ops[a];
        ops[a] = ops[nbOp - 1 - a];
        ops[nbOp - 1 - a] = tmp;
    }

    free(dp);
    free(back);

    return (Alignment){ .k = min_k, .nbOp = nbOp, .operations = ops };
}

// === Append Utility for Debug Strings ===
void append_char(char* s, char c) {
    size_t len = strlen(s);
    s[len] = c;
    s[len + 1] = '\0';
}

// === Candidate Verification by Approximate Matching ===
void verify_candidates(CandidateList* candidates_list, char* text, char* pattern, int k) {
    int text_len = strlen(text);
    int pattern_len = strlen(pattern);

    for (int i = 0; i < candidates_list->size; i++) {
        Candidate* cand = &candidates_list->candidates[i];
        int start_left = MAX(cand->start_text - cand->fragment.start - k, 0);

        char text_left[cand->start_text - start_left + 1];
        strncpy(text_left, text + start_left, cand->start_text - start_left);
        text_left[cand->start_text - start_left] = '\0';

        char pattern_left[cand->fragment.start + 1];
        strncpy(pattern_left, pattern, cand->fragment.start);
        pattern_left[cand->fragment.start] = '\0';

        strrev(text_left);
        strrev(pattern_left);
        Alignment align_left = edit_distance_aligned(text_left, pattern_left);
        int k_left = align_left.k;
        if (k_left == INF) continue;

        int k_right_max = k - k_left;
        int end_right = MIN(cand->end_text + (pattern_len - cand->fragment.end - 1) + k_left, text_len);

        char text_right[end_right - cand->end_text + 1];
        strncpy(text_right, text + cand->end_text, end_right - cand->end_text);
        text_right[end_right - cand->end_text] = '\0';

        char pattern_right[pattern_len - cand->fragment.end + 1];
        strncpy(pattern_right, pattern + cand->fragment.end, pattern_len - cand->fragment.end);
        pattern_right[pattern_len - cand->fragment.end] = '\0';

        Alignment align_right = edit_distance_aligned(text_right, pattern_right);
        int k_right = align_right.k;

        if (k_right > k_right_max) continue;

        printf("Candidate %d is valid with %d errors.\n", i, k_left + k_right);
        char t[100] = "", p[100] = "", op[100] = "";
        int i_p = 0, i_t = MAX(cand->start_text - cand->fragment.start - k_left , 0);

        for (int j = 0; j < align_left.nbOp; j++) {
            switch (align_left.operations[j]) {
                case DEL: append_char(t, '-'); append_char(p, pattern[i_p++]); append_char(op, ' '); break;
                case INS: append_char(t, text[i_t++]); append_char(p, '-'); append_char(op, ' '); break;
                case SUB: append_char(t, text[i_t++]); append_char(p, pattern[i_p++]); append_char(op, '|'); break;
                case EQ:  append_char(t, text[i_t++]); append_char(p, pattern[i_p++]); append_char(op, '|'); break;
            }
        }

        for (int j = 0; j < cand->fragment.end - cand->fragment.start; j++) {
            append_char(t, text[i_t]);
            append_char(p, pattern[i_p]);
            append_char(op, '|');
            i_t++; i_p++;
        }

        for (int j = 0; j < align_right.nbOp; j++) {
            switch (align_right.operations[j]) {
                case DEL: append_char(t, '-'); append_char(p, pattern[i_p++]); append_char(op, ' '); break;
                case INS: append_char(t, text[i_t++]); append_char(p, '-'); append_char(op, ' '); break;
                case SUB: append_char(t, text[i_t++]); append_char(p, pattern[i_p++]); append_char(op, '|'); break;
                case EQ:  append_char(t, text[i_t++]); append_char(p, pattern[i_p++]); append_char(op, '|'); break;
            }
        }

        printf("%s\n%s\n%s\n", t, op, p);
    }
}