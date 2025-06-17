// Approximate matching with Dynamic Programming over DNAs sequence.

#include "DynP.h"

int** createMatrix(char* sequence, char* pattern) {
    // Create and initialize a matrix for dynamic programming
    //      0   s   e   q   u   e   n   c   e
    //  0   0   0   0   0   0   0   0   0   0
    //  p   1   
    //  a   2   
    //  t   3  
    //  t   4
    //  e   5
    //  r   6
    //  n   7
    //
    //
    int seqLen = strlen(sequence);
    int patLen = strlen(pattern);
    int** matrix = (int**)malloc((patLen + 1) * sizeof(int*));
    for (int i = 0; i <= patLen; i++) {
        matrix[i] = (int*)malloc((seqLen + 1) * sizeof(int));
        matrix[i][0] = i;
        for (int j = 1; j <= seqLen; j++) {
            if (i == 0) {
                matrix[i][j] = 0;
            } else {
                matrix[i][j] = -1;
            }
        }
    }
    return matrix;
}

void freeMatrix(int** matrix, int rows) {
    // Free the allocated memory for the matrix
    for (int i = 0; i < rows; i++) {
        free(matrix[i]);
    }
    free(matrix);
}

int min(int a, int b, int c) {
    // Return the minimum of three integers
    if (a < b) {
        return (a < c) ? a : c;
    } else {
        return (b < c) ? b : c;
    }
}

void fillMatrix(int** matrix, char* sequence, char* pattern) {
    // Fill the matrix with dynamic programming values
    int seqLen = strlen(sequence);
    int patLen = strlen(pattern);

    for (int i = 1; i <= patLen; i++) {
        for (int j = 1; j <= seqLen; j++) {
            if (pattern[i-1] == sequence[j-1]) {
                matrix[i][j] = matrix[i - 1][j - 1];
            } else {
                matrix[i][j] = 1 + min(matrix[i - 1][j],    // Deletion
                                       matrix[i][j - 1],    // Insertion
                                       matrix[i - 1][j - 1]); // Substitution
            }
        }
    }
}



MatchListPtr approximateMatching(int** matrix, char* sequence, char* pattern, int maxErrors) {
    // Find approximate matches in the matrix
    MatchListPtr head = NULL;
    MatchListPtr tail = NULL;
    int seqLen = strlen(sequence);
    int patLen = strlen(pattern);

    for (int i = 0; i <= seqLen; i++) {
        if (matrix[patLen][i] >= 0 && matrix[patLen][i] <= maxErrors) {
            MatchInfo* match = (MatchInfo*)malloc(sizeof(MatchInfo));
            match->end = i;
            match->errors = matrix[patLen][i];

            MatchListPtr newNode = (MatchListPtr)malloc(sizeof(MatchList));
            newNode->match = match;
            newNode->next = NULL;

            if (head == NULL) {
                head = newNode; // First match found
                tail = newNode; // Initialize tail
            } else {
                tail->next = newNode; // Append to the list
                tail = newNode; // Update tail
            }
        }
    }
    return head;
}

void analyzeMatching(int** matrix, char* sequence, char* pattern, MatchInfo match) {
    int seqIndex = match.end - 1;
    int patIndex = strlen(pattern) - 1;
    int i = patIndex + match.errors + 1;
    char seq[i];
    char pat[i];
    char err[i];
    for (int j = 0; j < i; j++) {
        seq[j] = '\b'; // Initialize with backspace character
        pat[j] = '\b';
        err[j] = '\b';
    }
    seq[i] = '\0';
    pat[i] = '\0';
    err[i] = '\0';
    
    i--;
    int nb_err = match.errors;
    while (patIndex >= 0){
        if (sequence[seqIndex] == pattern[patIndex]) {
            seq[i] = sequence[seqIndex];
            pat[i] = pattern[patIndex];
            err[i] = '|';
            seqIndex--;
            patIndex--;
        }
        else if (matrix[patIndex][seqIndex] == 1 + matrix[patIndex - 1][seqIndex]) {
            seq[i] = '-';
            pat[i] = pattern[patIndex];
            err[i] = ' ';
            patIndex--;
        } else if (matrix[patIndex][seqIndex] == 1 + matrix[patIndex][seqIndex - 1]) {
            seq[i] = sequence[seqIndex];
            pat[i] = '-';
            err[i] = ' ';
            seqIndex--;
        } else {
            seq[i] = sequence[seqIndex];
            pat[i] = pattern[patIndex];
            err[i] = '*';
            seqIndex--;
            patIndex--;
            nb_err--;
        }
        i--;
    }
    printf("Match at end index %d with %d errors:\n", match.end, match.errors);
    printf("%s\n", seq);
    printf("%s\n", err);
    printf("%s\n", pat);
}

void printMatrix(int** matrix, int rows, int cols) {
    // Print the matrix for debugging purposes
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            printf("%d ", matrix[i][j]);
        }
        printf("\n");
    }
}

