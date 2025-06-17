#include <stdio.h>
#include <stdlib.h>
#include <string.h>


typedef enum { DEL, INS, SUB, EQ } Operation;

typedef struct {
    int k;
    int nbOp;
    Operation* operations;
} Alignment;

typedef struct fragment {
    int id;
    int start;
    int end;
    char* text;

} Fragment;

typedef struct fragment_list {
    Fragment* fragments;
    int size;
    int capacity;
} FragmentList;

typedef struct {
    Fragment fragment;
    int start_text;
    int end_text;
} Candidate;

typedef struct candidate_list {
    Candidate* candidates;
    int size;
    int capacity;
} CandidateList;



Alignment create_alignment() {
    Alignment alignment;
    alignment.k = 0;
    alignment.nbOp = 0;
    alignment.operations = NULL;
    return alignment;
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
    dest.fragment.text = strdup(src->fragment.text); // <-- ici
    dest.start_text = src->start_text;
    dest.end_text = src->end_text;
    return dest;
}

void free_alignment(Alignment* alignment) {
    free(alignment->operations);
    free(alignment);
}


void free_fragment_list(FragmentList* list) {
    for (int i = 0; i < list->size; i++) {
        free(list->fragments[i].text);
    }
    free(list->fragments);
    free(list);
}

void free_candidate_list(CandidateList* list) {
    for (int i = 0; i < list->size; i++) {
        free(list->candidates[i].fragment.text);
    }
    free(list->candidates);
    free(list);
}

void print_fragment_list(FragmentList* list) {
    for (int i = 0; i < list->size; i++) {
        printf("Fragment %d: '%s' from [%d, %d]\n",
               list->fragments[i].id, list->fragments[i].text,
               list->fragments[i].start, list->fragments[i].end);
    }
}

void print_candidate_list(CandidateList* list) {
    for (int i = 0; i < list->size; i++) {
        printf("Candidate %d: Fragment '%s' (%d) at text position [%d, %d]\n",
               i, list->candidates[i].fragment.text, list->candidates[i].fragment.id,
               list->candidates[i].start_text, list->candidates[i].end_text);
    }
}

void strrev(char *str) {
    int i = 0;
    int j = strlen(str) - 1;
    while (i < j) {
        char tmp = str[i];
        str[i] = str[j];
        str[j] = tmp;
        i++;
        j--;
    }
}


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
        if (i < nb_frag_high) {
            frag.end = frag.start + len_frag + 1;
            start += len_frag + 1;
            frag.text = malloc(sizeof(char) * (len_frag + 2));
            strncpy(frag.text, text + frag.start, len_frag + 1);
            frag.text[len_frag + 1] = '\0';
        } else {
            frag.end = frag.start + len_frag;
            start += len_frag;
            frag.text = malloc(sizeof(char) * (len_frag + 1));
            strncpy(frag.text, text + frag.start, len_frag);
            frag.text[len_frag] = '\0';;
        } 
        if (list->size >= list->capacity) {
            list->capacity *= 2;
            list->fragments = realloc(list->fragments, sizeof(Fragment) * list->capacity);
        }
        list->fragments[list->size++] = frag;
    }
    return list;
}



#define BASE1 33
#define PRIME1 1000003
#define BASE2 37
#define PRIME2 1000033

typedef struct {
    unsigned long long h1;
    unsigned long long h2;
} HashPair;

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

    // Prépare les puissances pour le rolling hash
    unsigned long long pow1 = 1, pow2 = 1;
    for (int i = 0; i < m - 1; i++) {
        pow1 = (pow1 * BASE1) % PRIME1;
        pow2 = (pow2 * BASE2) % PRIME2;
    }

    for (int i = 0; i <= n - m; i++) {
        if (txt_hash.h1 == pat_hash.h1 &&
            txt_hash.h2 == pat_hash.h2 &&
            strncmp(text + i, pattern, m) == 0) {

            Candidate cand;
            cand.fragment.start = frag.start;
            cand.fragment.end = frag.end;
            cand.fragment.text = strdup(pattern);
            cand.fragment.id = frag.id;
            cand.start_text = i;
            cand.end_text = i + m;

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




CandidateList* exact_match(char* text, Fragment frag) {
    CandidateList* candidates_list = create_candidate_list();
    rabin_karp(text, frag, candidates_list);                          //for example, use Rabin-Karp algorithm for exact matching
    return candidates_list;
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
    FragmentList* fragments_list = make_fragments(pattern, k);
    print_fragment_list(fragments_list);
    CandidateList* candidates_list = create_candidate_list();
    Fragment* frag = fragments_list->fragments;
    int frag_nb = fragments_list->size;
    for (int i = 0; i < frag_nb; i++) {
        CandidateList* exact_candidates = exact_match(text, frag[i]);
        add_candidates(candidates_list, exact_candidates);
        free_candidate_list(exact_candidates);
    }
    free_fragment_list(fragments_list);
    return candidates_list;
}

#define MIN(a,b) ((a)<(b)?(a):(b))
#define MAX(a,b) ((a)>(b)?(a):(b))

#define INF 999999
Alignment edit_distance_aligned(char* text, char* pattern, int k) {
    int m = strlen(pattern);
    int n = strlen(text);

    // Allocation de la matrice distance et des backpointers
    int** dp = malloc((m + 1) * sizeof(int*));
    Operation** back = malloc((m + 1) * sizeof(Operation*));
    for (int i = 0; i <= m; i++) {
        dp[i] = malloc((n + 1) * sizeof(int));
        back[i] = malloc((n + 1) * sizeof(Operation));
    }

    // Initialisation
    dp[0][0] = 0;
    back[0][0] = EQ;
    for (int j = 1; j <= n; j++) {
        dp[0][j] = INF;  // insertion interdite initialement
        back[0][j] = INS;
    }
    for (int i = 1; i <= m; i++) {
        dp[i][0] = i;
        back[i][0] = DEL;
    }

    // Remplissage de la matrice
    for (int i = 1; i <= m; i++) {
        for (int j = 1; j <= n; j++) {
            int cost_sub = (pattern[i - 1] == text[j - 1]) ? 0 : 1;

            int del = dp[i - 1][j] + 1;
            int ins = dp[i][j - 1] + 1;
            int sub = dp[i - 1][j - 1] + cost_sub;

            dp[i][j] = sub;
            back[i][j] = cost_sub ? SUB : EQ;

            if (del < dp[i][j]) {
                dp[i][j] = del;
                back[i][j] = DEL;
            }

            if (ins < dp[i][j]) {
                dp[i][j] = ins;
                back[i][j] = INS;
            }
        }
    }

    // Chercher le j final avec min dist
    int min_k = INF;
    int end_j = -1;
    for (int j = 0; j <= n; j++) {
        if (dp[m][j] < min_k) {
            min_k = dp[m][j];
            end_j = j;
        }
    }

    // Trace des opérations
    Operation* ops = malloc(sizeof(Operation) * (m + n));
    int nbOp = 0;
    int i = m, j = end_j;

    while (i > 0 || j > 0) {
        Operation op = back[i][j];
        ops[nbOp++] = op;
        if (op == EQ || op == SUB) {
            i--; j--;
        } else if (op == DEL) {
            i--;
        } else if (op == INS) {
            j--;
        }
    }

    // Inverser les opérations
    for (int a = 0; a < nbOp / 2; a++) {
        Operation tmp = ops[a];
        ops[a] = ops[nbOp - 1 - a];
        ops[nbOp - 1 - a] = tmp;
    }

    // Libération
    for (int i = 0; i <= m; i++) {
        free(dp[i]);
        free(back[i]);
    }
    free(dp);
    free(back);

    Alignment res;
    res.k = min_k;
    res.nbOp = nbOp;
    res.operations = ops;
    return res;
}


void append_char(char* s, char c) {
    size_t len = strlen(s);
    s[len] = c;
    s[len + 1] = '\0';
}


void verify_candidates(CandidateList* candidates_list, char* text, char* pattern, int k) {
    int text_len = strlen(text);
    int pattern_len = strlen(pattern);
    for (int i = 0; i < candidates_list->size; i++) {
        Candidate* cand = &candidates_list->candidates[i];
        int start_left = MAX(cand->start_text - cand->fragment.start - k,0);
        char text_left[cand->start_text - start_left + 1];
        strncpy(text_left, text + start_left, cand->start_text - start_left);
        text_left[cand->start_text - start_left] = '\0';
        char pattern_left[cand->fragment.start + 1];
        strncpy(pattern_left, pattern, cand->fragment.start);
        pattern_left[cand->fragment.start] = '\0';
        strrev(text_left);
        strrev(pattern_left);
        Alignment align_left = edit_distance_aligned(text_left, pattern_left, k);
        int k_left = align_left.k;
        if (k_left == INF) {
            continue; // Si la distance d'édition est trop grande, on passe au candidat suivant
        }

        int k_right_max = k - k_left;
        int end_right = MIN(cand->end_text + (pattern_len - cand->fragment.end - 1) + k_left, text_len);
        char text_right[end_right - cand->end_text + 1];
        strncpy(text_right, text + cand->end_text, end_right - cand->end_text);
        text_right[end_right - cand->end_text] = '\0';
        char pattern_right[pattern_len - cand->fragment.end + 1];
        strncpy(pattern_right, pattern + cand->fragment.end, pattern_len - cand->fragment.end);
        pattern_right[pattern_len - cand->fragment.end] = '\0';
        Alignment align_righ = edit_distance_aligned(text_right, pattern_right, k_right_max);
        int k_right = align_righ.k;
        if (k_right > k_right_max) {
            continue;
        }
        else {
            // Si les deux distances d'édition sont acceptables, on garde le candidat
            printf("Candidate %d is valid with %d errors.\n", i, k_left + k_right);
            char t[100] = "";
            char p[100] = "";
            char op[100] = "";
            int id_text = MAX(cand->start_text - cand->fragment.start - k_left , 0);
            int i_p = 0;
            int i_t = 0;
            for (int j = 0; j < align_left.nbOp; j++) {
                switch (align_left.operations[j]) {
                    case DEL: append_char(t, '-'); append_char(p, pattern[i_p]); i_p++; append_char(op, ' '); break;
                    case INS: append_char(t, text[i_t]); i_t++; append_char(p, '-'); append_char(op, ' '); break;
                    case SUB: append_char(t ,text[i_t]); i_t++; append_char(p, pattern[i_p]); i_p++; append_char(op, '|'); break;
                    case EQ: append_char(t ,text[i_t]); i_t++; append_char(p, pattern[i_p]); i_p++; append_char(op, '|');
                }
            }
            for (int j = 0; j < cand->fragment.end - cand->fragment.start; j++) {
                append_char(t ,text[i_t]);
                i_t++;
                append_char(p, pattern[i_p]);
                i_p++;
                append_char(op, '|');
            }
            for (int j = 0; j < align_righ.nbOp; j++) {
                switch (align_righ.operations[j]) {
                    case DEL: append_char(t, '-'); append_char(p, pattern[i]); i_p++; append_char(op, ' '); break;
                    case INS: append_char(t, text[i_t]); i_t++; append_char(p, '-'); append_char(op, ' '); break;
                    case SUB: append_char(t ,text[i_t]); i_t++; append_char(p, pattern[i_p]); i_p++; append_char(op, '|'); break;
                    case EQ: append_char(t ,text[i_t]); i_t++; append_char(p, pattern[i_p]); i_p++; append_char(op, '|');
                }
            }
            printf("%s\n%s\n%s\n", t, op, p);
        }

    }
}

int main() {
    /*char text[100];
    char pattern[100];
    printf("Enter the text: ");
    scanf("%s", text);
    printf("Enter the pattern: ");
    scanf("%s", pattern);
    int k;
    printf("Enter the maximum number of errors allowed: ");
    scanf("%d", &k);
    */
    char* text = "ACTATGTCATGC";
    char* pattern = "ACGGTCA";
    int k = 2;
    CandidateList* candidates_list = find_candidates(text, pattern, k);
    print_candidate_list(candidates_list);
    verify_candidates(candidates_list, text, pattern, k);
    free_candidate_list(candidates_list);
    return 0;
}