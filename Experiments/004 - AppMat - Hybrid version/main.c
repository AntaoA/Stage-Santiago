#include "hybridAM.h"

int main() {
    /*char text[100], pattern[100];
    int k, j;

    // Input sequence and pattern
    printf("Enter the sequence: ");
    scanf("%s", text);
    printf("Enter the pattern: ");
    scanf("%s", pattern);

    // Input allowed errors and number of fragments
    printf("Enter the maximum number of errors allowed: ");
    scanf("%d", &k);
    printf("Enter the number of fragments: ");
    scanf("%d", &j);*/

    char* text = "zoigjzeiofjzeiojfozijfziofjzeiofj";
    char* pattern = "zoeifjzioejzef";
    int k = 6;
    int j = 4;

    // Create suffix trie from the text
    SuffixTrie* trie = st_create(text);
    st_print_compressed(trie);
    printf("\n");

    // Fragment the pattern into j parts → j-1 cuts
    FragmentsList* fragments = make_fragments(pattern, j - 1);

    // First stage: ST-based matching with k/j error tolerance
    int k_first = k / j;
    MatchListArray matchesArray = create_match_list_array(j);
    hybrid_partial_match(matchesArray, fragments, k_first, trie);

    // Convert match results to candidate list with alignments
    CandidatesWithErrors* candidatesErrors = convert_matches_to_candidates_with_errors(matchesArray, fragments);

    // Print and verify all candidate matches
    print_candidates_with_errors(candidatesErrors);
    verify_candidates_with_errors(candidatesErrors, text, pattern, k);

    // Free memory
    free_candidates_with_errors(candidatesErrors);
    free_match_list_array(matchesArray);
    free_fragment_list(fragments);
    st_destroy(trie);

    return 0;
}
