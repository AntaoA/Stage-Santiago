#ifndef SUFFIX_TRIE_H
#define SUFFIX_TRIE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//
// Data Structures
//

// Node of the compressed suffix trie
typedef struct TrieNode {
    char *labels;               // labels for each edge to children
    struct TrieNode **children; // pointers to child nodes
    int childCount;            // number of children currently used
    int capacity;              // allocated size for labels/children
    int suffixIndex;          // -1 for internal node, >= 0 for leaf (suffix starting at that index)
} TrieNode;

// Full suffix trie structure
typedef struct {
    char *text;                // input text (must be null-terminated)
    TrieNode *root;            // root of the trie
    int size;                  // length of the input text
} SuffixTrie;

//
// Function Declarations
//

// Node creation and management
TrieNode *new_trie_node();
TrieNode *find_child(TrieNode *node, char c);
TrieNode *add_child(TrieNode *node, char c, TrieNode *child);
void st_free_node(TrieNode *node);

// Suffix trie API
SuffixTrie *st_create(char *text);
void st_destroy(SuffixTrie *st);

// Printing (for debugging or visualization)
void st_print_node_compressed(TrieNode *node, char *text, char *prefix, int isLast, char edgeChar);
void st_print_compressed(SuffixTrie *st);

#endif // SUFFIX_TRIE_H