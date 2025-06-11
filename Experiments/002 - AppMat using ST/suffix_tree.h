#ifndef SUFFIX_TREE_H
#define SUFFIX_TREE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_CHAR 256

// Node of the suffix tree
typedef struct SuffixNode {
    struct SuffixNode *children[MAX_CHAR];
    struct SuffixNode *suffixLink;
    int start;
    int *end;
    int suffixIndex;
} SuffixNode;

// Encapsulated suffix tree object
typedef struct SuffixTree {
    char *text;                // original text
    int size;                  // length of text
    SuffixNode *root;          // root of tree

    // Active point for Ukkonen
    SuffixNode *lastNewNode;
    SuffixNode *activeNode;
    int activeEdge;
    int activeLength;

    int remainingSuffixCount;  // remaining suffixes to add
    int leafEnd;               // global leaf end index
    int *rootEnd;
} SuffixTree;

// Build suffix tree for `text` (must end with '$')
SuffixTree *st_create(const char *text);
// Print the tree for debugging
void st_print(SuffixTree *st);
// Destroy and free all memory
void st_destroy(SuffixTree *st);

#endif // SUFFIX_TREE_H