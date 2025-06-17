#define _GNU_SOURCE
#include "rabin_karp.h"

// === Rabin-Karp Hashing and Matching ===
HashPair double_hash(char* str, int len) {
    HashPair hp = {0, 0};
    for (int i = 0; i < len; i++) {
        unsigned char c = (unsigned char)str[i];
        hp.h1 = (hp.h1 * BASE1 + c) % PRIME1;
        hp.h2 = (hp.h2 * BASE2 + c) % PRIME2;
    }
    return hp;
}


void rabin_karp(char* text, Fragment frag, CandidateList* candidates_list) {
    char* pattern = frag.text;
    int n = strlen(text);
    int m = strlen(pattern);
    if (m > n) return;

    HashPair pat_hash = double_hash(pattern, m);
    HashPair txt_hash = double_hash(text, m);

    unsigned long long pow1 = 1, pow2 = 1;
    for (int i = 0; i < m - 1; i++) {
        pow1 = (pow1 * BASE1) % PRIME1;
        pow2 = (pow2 * BASE2) % PRIME2;
    }

    for (int i = 0; i <= n - m; i++) {
        if (txt_hash.h1 == pat_hash.h1 &&
            txt_hash.h2 == pat_hash.h2 &&
            strncmp(text + i, pattern, m) == 0) {

            Candidate cand = {
                .fragment = { frag.id, frag.start, frag.end, strdup(pattern) },
                .start_text = i,
                .end_text = i + m
            };

            if (candidates_list->size >= candidates_list->capacity) {
                candidates_list->capacity *= 2;
                candidates_list->candidates = realloc(candidates_list->candidates,
                                                      sizeof(Candidate) * candidates_list->capacity);
            }
            candidates_list->candidates[candidates_list->size++] = cand;
        }

        if (i < n - m) {
            unsigned char left = (unsigned char)text[i];
            unsigned char right = (unsigned char)text[i + m];
            txt_hash.h1 = (BASE1 * (txt_hash.h1 + PRIME1 - (left * pow1) % PRIME1) + right) % PRIME1;
            txt_hash.h2 = (BASE2 * (txt_hash.h2 + PRIME2 - (left * pow2) % PRIME2) + right) % PRIME2;
        }
    }
}