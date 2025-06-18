#ifndef A25672D3_A366_4E77_AB6D_68E8826509DF
#define A25672D3_A366_4E77_AB6D_68E8826509DF

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    int end;    // End index of the match in the sequence
    int errors; // Number of errors in the match
} MatchInfo;

typedef struct MatchInfoList {
    MatchInfo* match; // Pointer to the match information
    struct MatchInfoList* next; // Pointer to the next match in the list
} MatchInfoList;

typedef MatchInfoList* MatchInfoListPtr;

int** createMatrix(char* sequence, char* pattern);
void fillMatrix(int** matrix, char* sequence, char* pattern);
void printMatrix(int** matrix, int rows, int cols);
void freeMatrix(int** matrix, int rows);
MatchInfoListPtr approximateMatchingDP(int** matrix, char* sequence, char* pattern, int maxErrors);
void analyzeMatching(int** matrix, char* sequence, char* pattern, MatchInfo match);
int min(int a, int b, int c);


#endif /* A25672D3_A366_4E77_AB6D_68E8826509DF */
