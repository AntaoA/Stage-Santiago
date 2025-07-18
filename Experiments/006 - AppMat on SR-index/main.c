#include "../005 - SR-index/sr-index.h"
#include "fragments_candidates.h"
#include "verify.h"


#define MIN(a,b) ((a)<(b)?(a):(b))
#define MAX(a,b) ((a)>(b)?(a):(b))
#define INF 999999

int main() {
    const char* input = "ALABARALAALABARDA$";
    int s = 2; // Subsampling parameter
    srIndex* index = create_sr_index(input, s);
    if (index == NULL) {
        fprintf(stderr, "Error creating the index.\n");
        return EXIT_FAILURE;
    }
    printf("Index created successfully.\n");
    char pattern[] = "ALABRADB";
    //char pattern[] = "BLADARBA";
    int pattern_length = strlen(pattern);
    int k = 3; // Maximum allowed errors
    int j = 2;
    int k_first = k / j;

    printf("Counting approximate occurrences of pattern '%s' with up to %d errors...\n\n\n", pattern, k);


    candidates* cand = create_candidates(index, pattern, k_first, j-1);

    for (int i = 0; i < j; i++) {
        printf("Fragment %d: '%s'\n", cand[i].id, cand[i].text);
        if (cand[i].matches->match_count > 0) {
            printf("Matches found:\n");
            for (int m = 0; m < cand[i].matches->match_count; m++) {
                printf("Position: %d, Errors: %d\n", cand[i].matches->matches[m].position, cand[i].matches->matches[m].errors);
            }
            printf("\n");
        } else {
            printf("No matches found.\n\n");
        }
    }


    char pat[pattern_length + 1];
    for (int m = 0; m < pattern_length; m++) {
        pat[m] = pattern[m];
    }
    pat[pattern_length] = '\0';
    
    int id_left = 0;
    int id_right = 0;


    printf("Verifying candidates...\n\n\n");


    //verify candidates
    for (int i = 0; i < j; i++) {
        int nb_matches = cand[i].matches ? cand[i].matches->match_count : 0;
        approx_matching* approx_matches = cand[i].matches;
        id_right += strlen(cand[i].text);
        for (int m = 0; m < nb_matches; m++) {
            match* match = &approx_matches->matches[m];
            int k_frag = match->errors;
            int k_left_max = k - k_frag;
            printf("Verifying match at position %d in fragment %d with %d errors. Position_sa = %d\n", match->position, i, k_frag, match->position_sa);
            verification_result left_result = verify_left(index, match->position, match->position_sa, k_left_max, pat, id_left);
            int position_start = left_result.position_start;
            int k_right_max = left_result.err_max;
            printf("Left verification terminated.\n");
            if (k_right_max < 0) {
                printf("No valid left verification found for match at position %d in fragment %d.\n\n", match->position, i);
                continue;
            }
            printf("Right verification with max errors %d.\n", k_right_max);
            int err = verify_right(index, match->position, match->position_sa, k_left_max, pat, id_right, match->nb_letters_seen_in_text);

            if (err >= 0) {
                printf("Match at position %d in fragment %d verified successfully with %d errors.\n\n", match->position, i, err);
            } 
            else {
                printf("Match at position %d in fragment %d failed verification.\n\n", match->position, i);
            }
        }
        id_left += strlen(cand[i].text);
    }



    // Free allocated memory
    free(index->start);
    free(index->letters);
    free(index->lsa.LSA);
    free(index->del);
    free(index->FT);
    free(index->mapFL);
    free(index->C);
    free(index->LF);
    free(index);

    return EXIT_SUCCESS;
}