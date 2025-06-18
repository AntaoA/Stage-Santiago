#include "fragmentsAM.h"

int main() {
    char text[100], pattern[100];
    int k;

    printf("Enter the text: ");
    scanf("%s", text);
    printf("Enter the pattern: ");
    scanf("%s", pattern);
    printf("Enter the maximum number of errors allowed: ");
    scanf("%d", &k);

    CandidatesList* candidates = find_candidates(text, pattern, k);
    print_candidate_list(candidates);
    verify_candidates(candidates, text, pattern, k);
    free_candidate_list(candidates);
    return 0;
}