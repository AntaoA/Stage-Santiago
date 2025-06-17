#ifndef A25672D3_A366_4E77_AB6D_68E8826509DF
#define A25672D3_A366_4E77_AB6D_68E8826509DF

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    int end;    // End index of the match in the sequence
    int errors; // Number of errors in the match
} MatchInfo;

typedef struct MatchList {
    MatchInfo* match; // Pointer to the match information
    struct MatchList* next; // Pointer to the next match in the list
} MatchList;

typedef MatchList* MatchListPtr;

int** createMatrix(char* sequence, char* pattern);
void fillMatrix(int** matrix, char* sequence, char* pattern);
void printMatrix(int** matrix, int rows, int cols);
void freeMatrix(int** matrix, int rows);
MatchListPtr approximateMatching(int** matrix, char* sequence, char* pattern, int maxErrors);
void analyzeMatching(int** matrix, char* sequence, char* pattern, MatchInfo match);
int min(int a, int b, int c);


#endif /* A25672D3_A366_4E77_AB6D_68E8826509DF */
