#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct TrieNode {
    char *labels;              // tableau des caractères étiquetant chaque enfant
    struct TrieNode **children; // tableau des pointeurs enfants
    int numChildren;           // nombre d'enfants
    int capacity;              // capacité allouée
    int suffixIndex;           // -1 pour noeud interne, sinon position du suffixe
} TrieNode;

typedef struct {
    char *text;
    TrieNode *root;
    int size;
} SuffixTrie;



TrieNode *new_trie_node();
TrieNode *find_child(TrieNode *node, char c);
TrieNode *add_child(TrieNode *node, char c, TrieNode *child);
SuffixTrie *st_create(char *text);
void st_free_node(TrieNode *node);
void st_destroy(SuffixTrie *st);
void st_print_node_compressed(TrieNode *node, char *text, char *prefix, int isLast, char edgeChar);
void st_print_compressed(SuffixTrie *st);