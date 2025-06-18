#define _GNU_SOURCE
#include "hybridAM.h"

/// Frees all memory used by a CandidatesWithErrors structure
void free_candidates_with_errors(CandidatesWithErrors* candidatesErrors) {
    for (int i = 0; i < candidatesErrors->candidates->size; i++)
        free(candidatesErrors->align[i].operations);
    free(candidatesErrors->align);
    free(candidatesErrors->candidates->candidate);
    free(candidatesErrors->candidates);
    free(candidatesErrors);
}

/// Creates a MatchListArray for k patterns
MatchListArray create_match_list_array(int k) {
    MatchListArray array;
    array.data = malloc(sizeof(MatchListWithPattern) * k);
    array.size = k;
    return array;
}

/// Frees memory used by a MatchListArray
void free_match_list_array(MatchListArray array) {
    for (int i = 0; i < array.size; i++) {
        free(array.data[i].pattern);
        freeMatchList(array.data[i].matchList);
    }
    free(array.data);
}

/// Prints each pattern with its associated matches
void print_match_list_multiple_patterns(MatchListArray matchesArray, char* text) {
    for (int i = 0; i < matchesArray.size; i++) {
        printf("Pattern: %s\n", matchesArray.data[i].pattern);
        printMatchList(matchesArray.data[i].matchList, text, matchesArray.data[i].pattern);
    }
}

/// Copies a MatchList deeply
MatchList* copy_match_list(MatchList* src) {
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

/// Runs approximate matching via suffix trie for all fragments
void hybrid_partial_match(MatchListArray matchesArray, FragmentsList* frags, int k, SuffixTrie* st) {
    for (int i = 0; i < frags->size; i++) {
        Fragment frag = frags->fragment[i];
        printf("Processing fragment %d: %s\n", frag.id, frag.text);
        MatchList* partialMatches = createMatchList();
        approximateMatching(st, frag.text, k, partialMatches);
        matchesArray.data[i].matchList = copy_match_list(partialMatches);
        matchesArray.data[i].pattern = strdup(frag.text);
        freeMatchList(partialMatches);
    }
    print_match_list_multiple_patterns(matchesArray, st->text);
}

/// Converts int to Operation enum
Operation get_operation_type(int op) {
    switch (op) {
        case INS: return I;
        case DEL: return D;
        case SUB: return S;
        case EQ:  return E;
        default:  return -1;
    }
}

/// Converts an opArray to Alignment structure
Alignment* convert_op_array_to_alignment(opArray ops) {
    Alignment* align = malloc(sizeof(Alignment));
    align->k = 0;
    align->nbOp = ops.nbOp;
    align->operations = malloc(sizeof(Operation) * ops.nbOp);
    for (int i = 0; i < ops.nbOp; i++) {
        align->operations[i] = get_operation_type(ops.operations[i]);
        if (align->operations[i] != E) align->k++;
    }
    return align;
}

/// Converts all match lists to candidates with alignments
CandidatesWithErrors* convert_matches_to_candidates_with_errors(MatchListArray matchesArray, FragmentsList* fragments) {
    CandidatesList* candidates = create_candidate_list();
    CandidatesWithErrors* result = malloc(sizeof(CandidatesWithErrors));
    result->candidates = candidates;

    int total = 0;
    for (int i = 0; i < matchesArray.size; i++) {
        Fragment fragment = fragments->fragment[i];
        MatchList* list = matchesArray.data[i].matchList;
        for (int j = 0; j < list->size; j++) {
            total++;
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

    result->align = malloc(sizeof(Alignment) * total);
    int idx = 0;
    for (int i = 0; i < matchesArray.size; i++) {
        MatchList* list = matchesArray.data[i].matchList;
        for (int j = 0; j < list->size; j++) {
            Match m = list->data[j];
            result->align[idx].k = m.nbModifs;
            result->align[idx].nbOp = m.ops.nbOp;
            result->align[idx].operations = malloc(sizeof(Operation) * m.ops.nbOp);
            for (int k = 0; k < m.ops.nbOp; k++) {
                result->align[idx].operations[k] = get_operation_type(m.ops.operations[k]);
            }
            idx++;
        }
    }

    return result;
}

/// Prints a summary of candidate fragments and their errors
void print_candidates_with_errors(CandidatesWithErrors* candidatesErrors) {
    printf("Candidates with errors:\n");
    for (int i = 0; i < candidatesErrors->candidates->size; i++) {
        Candidate* cand = &candidatesErrors->candidates->candidate[i];
        printf("Fragment ID: %d, Start: %d, End: %d, Text: %s, Errors: %d\n",
               cand->fragment.id, cand->start_text, cand->end_text,
               cand->fragment.text, candidatesErrors->align[i].k);
    }
}

/// Re-verifies candidates by computing full alignment (left + middle + right)
void verify_candidates_with_errors(CandidatesWithErrors* candidatesErrors, char* text, char* pattern, int k) {
    int text_len = strlen(text);
    int pattern_len = strlen(pattern);
    CandidatesList* candidates = candidatesErrors->candidates;

    for (int i = 0; i < candidates->size; i++) {
        Candidate* cand = &candidates->candidate[i];
        int k_frag = candidatesErrors->align[i].k;
        int k_left_max = k - k_frag;

        // Left alignment
        int start_left = MAX(cand->start_text - cand->fragment.start - k_left_max, 0);
        char text_left[cand->start_text - start_left + 1];
        strncpy(text_left, text + start_left, cand->start_text - start_left);
        text_left[cand->start_text - start_left] = '\0';

        char pattern_left[cand->fragment.start + 1];
        strncpy(pattern_left, pattern, cand->fragment.start);
        pattern_left[cand->fragment.start] = '\0';

        strrev(text_left); strrev(pattern_left);
        Alignment* align_left = edit_distance_aligned(text_left, pattern_left);
        revOperations(align_left);

        if (align_left->k > k_left_max) continue;

        // Right alignment
        int k_right_max = k_left_max - align_left->k;
        int end_right = MIN(cand->end_text + (pattern_len - cand->fragment.end - 1) + k_right_max, text_len);

        char text_right[end_right - cand->end_text + 1];
        strncpy(text_right, text + cand->end_text + 1, end_right - cand->end_text);
        text_right[end_right - cand->end_text] = '\0';

        char pattern_right[pattern_len - cand->fragment.end];
        strncpy(pattern_right, pattern + cand->fragment.end + 1, pattern_len - cand->fragment.end);
        pattern_right[pattern_len - cand->fragment.end - 1] = '\0';

        Alignment* align_right = edit_distance_aligned(text_right, pattern_right);
        if (align_right->k > k_right_max) continue;

        // Print result
        printf("Candidate %d is valid with %d errors.\n", i, align_left->k + k_frag + align_right->k);

        char t[100] = "", p[100] = "", op[100] = "";
        int i_p = 0;
        int i_t = getStartPositionAlignment(align_left, cand->start_text - 1);

        for (int j = 0; j < align_left->nbOp; j++) {
            switch (align_left->operations[j]) {
                case D: append_char(t, '-'); append_char(p, pattern[i_p++]); append_char(op, ' '); break;
                case I: append_char(t, text[i_t++]); append_char(p, '-'); append_char(op, ' '); break;
                case S: append_char(t, text[i_t++]); append_char(p, pattern[i_p++]); append_char(op, '*'); break;
                case E: append_char(t, text[i_t++]); append_char(p, pattern[i_p++]); append_char(op, '|'); break;
            }
        }
        for (int j = 0; j < candidatesErrors->align[i].nbOp; j++) {
            switch (candidatesErrors->align[i].operations[j]) {
                case D: append_char(t, '-'); append_char(p, pattern[i_p++]); append_char(op, ' '); break;
                case I: append_char(t, text[i_t++]); append_char(p, '-'); append_char(op, ' '); break;
                case S: append_char(t, text[i_t++]); append_char(p, pattern[i_p++]); append_char(op, '*'); break;
                case E: append_char(t, text[i_t++]); append_char(p, pattern[i_p++]); append_char(op, '|'); break;
            }
        }
        for (int j = 0; j < align_right->nbOp; j++) {
            switch (align_right->operations[j]) {
                case D: append_char(t, '-'); append_char(p, pattern[i_p++]); append_char(op, ' '); break;
                case I: append_char(t, text[i_t++]); append_char(p, '-'); append_char(op, ' '); break;
                case S: append_char(t, text[i_t++]); append_char(p, pattern[i_p++]); append_char(op, '*'); break;
                case E: append_char(t, text[i_t++]); append_char(p, pattern[i_p++]); append_char(op, '|'); break;
            }
        }

        printf("%s\n%s\n%s\n", t, op, p);
    }
}
