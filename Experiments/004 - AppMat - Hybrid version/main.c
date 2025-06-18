#define _GNU_SOURCE
#include "../002 - AppMat using ST/STforAppMat.h"
#include "../003 - AppMat using fragments/fragmentsAM.h"
#include <string.h>

typedef struct MatchListPattern {
    MatchList* matchList;
    char* pattern;
} MatchListPattern;

typedef struct {
    MatchListPattern* data;
    int size;
} MatchListArray;

typedef struct CandidatesList_withErrors {
    CandidatesList* candidates;
    Alignment* align;
} CandidatesList_withErrors;



void freeCandidatesList_withErrors(CandidatesList_withErrors* candidatesErrors) {
    if (candidatesErrors) {
        free(candidatesErrors->candidates->candidate);
        free(candidatesErrors->candidates);
        for (int i = 0; i < candidatesErrors->candidates->size; i++) {
            free(candidatesErrors->align[i].operations);
        }
        free(candidatesErrors->align);
        free(candidatesErrors);
    }
}

MatchListArray createMatchListArray(int k) {
    MatchListArray array;
    array.data = malloc(sizeof(MatchListPattern) * k);
    array.size = k;
    return array;
}

void freeMatchListArray(MatchListArray array) {
    for (int i = 0; i < array.size; i++) {
        free(array.data[i].pattern);
        freeMatchList(array.data[i].matchList);
    }
    free(array.data);
}


void printMatchList_severalPatterns(MatchListArray matchesArray, char* text) {
    for (int i = 0; i < matchesArray.size; i++) {
        MatchList* list = matchesArray.data[i].matchList;
        printf("Pattern: %s\n", matchesArray.data[i].pattern);
        printMatchList(list, text, matchesArray.data[i].pattern);
    }
}

MatchList* copyMatchList(MatchList* src) {
    MatchList* dest = createMatchList();
    dest->size = src->size;
    dest->capacity = src->capacity;
    dest->data = malloc(sizeof(Match) * dest->capacity);
    for (int i = 0; i < src->size; i++) {
        dest->data[i].endPosition = src->data[i].endPosition;
        dest->data[i].nbModifs = src->data[i].nbModifs;
        dest->data[i].ops = copyOpArray(src->data[i].ops);
    }
    return dest;
}

void partial_match(MatchListArray matchesArray, FragmentsList* frags, int k, SuffixTrie* st) {
    Fragment frag;
    for (int i = 0; i < frags->size; i++) {
        frag = frags->fragment[i];
        printf("Processing fragment %d: %s\n", frag.id, frag.text);
        MatchList* partialMatches = createMatchList();
        approximateMatching(st, frag.text, k, partialMatches);
        matchesArray.data[i].matchList = copyMatchList(partialMatches);
        matchesArray.data[i].pattern = strdup(frag.text);
        freeMatchList(partialMatches);
    }
    printMatchList_severalPatterns(matchesArray, st->text);
}

Operation getOperationType(int op) {
    switch (op) {
        case INS: return I;
        case DEL: return D;
        case SUB: return S;
        case EQ: return E;
        default: return -1; // Invalid operation
    }
}

Alignment* opArray_to_alignment(opArray ops) {
    Alignment* align = malloc(sizeof(Alignment));
    align->k = 0;
    align->nbOp = ops.nbOp;
    align->operations = malloc(sizeof(Operation) * ops.nbOp);
    for (int i = 0; i < ops.nbOp; i++) {
        align->operations[i] = getOperationType(ops.operations[i]);
        if (align->operations[i] != E) {
            align->k++; // Increment edit distance for substitutions and equalities
        }
    }
    return align;
}

CandidatesList_withErrors* matchesList_to_candidatesListErrors(MatchListArray matchesArray, FragmentsList* fragments) {
    CandidatesList* candidates = create_candidate_list();
    CandidatesList_withErrors* candidatesErrors = malloc(sizeof(CandidatesList_withErrors));
    candidatesErrors->candidates = candidates;
    int len = 0;
    for (int i = 0; i < matchesArray.size; i++) {
        MatchList* list = matchesArray.data[i].matchList;
        Fragment fragment = fragments->fragment[i];
        for (int j = 0; j < list->size; j++) {
            len++;
            Match m = list->data[j];
            Candidate cand;
            cand.fragment = fragment;
            cand.start_text = getStartPositionOpArray(m.ops, m.endPosition);
            cand.end_text = m.endPosition;
            
            if (candidates->size >= candidates->capacity) {
                candidates->capacity *= 2;
                candidates->candidate = realloc(candidates->candidate, sizeof(Candidate) * candidates->capacity);
            }
            candidates->candidate[candidates->size++] = cand;
        }
    }
    candidatesErrors->align = malloc(sizeof(Alignment) * len);
    int index = 0;
    for (int i = 0; i < matchesArray.size; i++) {
        MatchList* list = matchesArray.data[i].matchList;
        for (int j = 0; j < list->size; j++) {
            Match m = list->data[j];
            candidatesErrors->align[index].k = m.nbModifs;
            candidatesErrors->align[index].nbOp = m.ops.nbOp;
            candidatesErrors->align[index].operations = malloc(sizeof(Operation) * m.ops.nbOp);
            for (int k = 0; k < m.ops.nbOp; k++) {
                candidatesErrors->align[index].operations[k] = getOperationType(m.ops.operations[k]);
            }
            index++;
        }
    }

    return candidatesErrors;
}

void print_candidate_list_errors(CandidatesList_withErrors* candidatesErrors) {
    printf("Candidates with errors:\n");
    for (int i = 0; i < candidatesErrors->candidates->size; i++) {
        Candidate* cand = &candidatesErrors->candidates->candidate[i];
        printf("Fragment ID: %d, Start: %d, End: %d, Text: %s, Errors: %d\n",
               cand->fragment.id, cand->start_text, cand->end_text, cand->fragment.text,
               candidatesErrors->align[i].k);
    }
}


void verify_candidates_errors(CandidatesList_withErrors* candidates_list_errors, char* text, char* pattern, int k) {
    int text_len = strlen(text);
    int pattern_len = strlen(pattern);
    CandidatesList* candidates_list = candidates_list_errors->candidates;
    for (int i = 0; i < candidates_list->size; i++) {

        int k_left_max = k - candidates_list_errors->align[i].k;
        Candidate* cand = &candidates_list->candidate[i];
        int start_left = MAX(cand->start_text - cand->fragment.start - k_left_max, 0);

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
        if (k_left > k_left_max) continue;
        int k_right_max = k_left_max - k_left;
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

        printf("Candidate %d is valid with %d errors.\n", i, k_left + k_right + candidates_list_errors->align[i].k);
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

        for (int j = 0; j < candidates_list_errors->align[i].nbOp; j++) {
            switch (candidates_list_errors->align[i].operations[j]) {
                case D: append_char(t, '-'); append_char(p, pattern[i_p++]); append_char(op, ' '); break;
                case I: append_char(t, text[i_t++]); append_char(p, '-'); append_char(op, ' '); break;
                case S: append_char(t, text[i_t++]); append_char(p, pattern[i_p++]); append_char(op, '*'); break;
                case E:  append_char(t, text[i_t++]); append_char(p, pattern[i_p++]); append_char(op, '|'); break;
            }
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

int main() {
    char text[100], pattern[100];
    printf("Enter the sequence: ");
    scanf("%s", text);
    printf("Enter the pattern: ");
    scanf("%s", pattern);

    int k, j;
    printf("Enter the maximum number of errors allowed: ");
    scanf("%d", &k);
    printf("Enter the number of fragments: ");
    scanf("%d", &j);


    SuffixTrie* st = st_create(text);
    st_print_compressed(st);
    printf("\n");

    FragmentsList* fragments = make_fragments(pattern, j-1);


    int k_first = k / j;
    MatchListArray matchesArray = createMatchListArray(j);
    partial_match(matchesArray, fragments, k_first, st);
    CandidatesList_withErrors* candidatesErrors = matchesList_to_candidatesListErrors(matchesArray, fragments);
    print_candidate_list_errors(candidatesErrors);
    verify_candidates_errors(candidatesErrors, text, pattern, k);
    
}