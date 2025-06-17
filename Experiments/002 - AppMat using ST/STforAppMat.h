#ifndef DAEC3258_9D15_46A3_96F1_4AEE00B2AA11
#define DAEC3258_9D15_46A3_96F1_4AEE00B2AA11


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "suffix_trie.h"




typedef enum { INS, DEL, SUB, EQ } operation;

typedef struct {
    int nbOp;
    operation* operations;
} opArray;

typedef struct {
    int* array;
    opArray* op;
    int len;
} distArray;

typedef struct {
    int endPosition;
    int nbModifs;
    opArray ops;
} Match;

typedef struct {
    Match* data;
    int size;
    int capacity;
} MatchList;


MatchList* createMatchList();
void addOrUpdateMatch(MatchList* list, int pos, int nbModifs, opArray ops);
int cmpMatch(const void* a, const void* b);
void printMatchList(MatchList* list, char* text, char* pattern);
void freeMatchList(MatchList* list);
void freeOpArray(opArray o);
opArray copyOpArray(opArray o);
void majDistArray(char c, char* pattern, distArray d);
distArray initArray(int len);
void freeDistArray(distArray* d);
void dfs_approximate_matching(TrieNode* node, char* text, char* pattern, int k, distArray d, int depth, MatchList* list);
int get_match_position(TrieNode* node);
void approximateMatching(SuffixTrie* st, char* pattern, int k);
distArray copyDistArray(distArray d);




#endif /* DAEC3258_9D15_46A3_96F1_4AEE00B2AA11 */
