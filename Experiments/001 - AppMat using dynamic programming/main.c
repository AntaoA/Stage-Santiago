#include "DynP.h"

int main() {
    char sequence[100], pattern[100];
    printf("Enter the sequence: ");
    scanf("%s", sequence);
    printf("Enter the pattern: ");
    scanf("%s", pattern);

    int** matrix = createMatrix(sequence, pattern);
    int seqLen = strlen(sequence);
    int patLen = strlen(pattern);

    printf("Initial Matrix:\n");
    printMatrix(matrix, patLen + 1, seqLen + 1);
    printf("\n");


    fillMatrix(matrix, sequence, pattern);
    printf("Filled Matrix:\n");
    printMatrix(matrix, patLen + 1, seqLen + 1);
    printf("\n");

    int maxErrors;
    printf("Enter the maximum number of errors allowed: ");
    scanf("%d", &maxErrors);

    MatchListPtr matches = approximateMatching(matrix, sequence, pattern, maxErrors);
    printf("Approximate Matches:\n");
    while (matches != NULL) {
        printf("End: %d, Errors: %d\n", matches->match->end, matches->match->errors);
        analyzeMatching(matrix, sequence, pattern, *(matches->match));
        MatchListPtr temp = matches;
        matches = matches->next;
        free(temp->match);
        free(temp);
    }
    
    freeMatrix(matrix, patLen + 1);
    return 0;
}