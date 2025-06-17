#define _GNU_SOURCE
#include "suffix_trie.h"

//
// TrieNode Creation and Management
//

// Allocate and initialize a new empty trie node
TrieNode *new_trie_node() {
    TrieNode *node = malloc(sizeof(TrieNode));
    node->childCount = 0;
    node->capacity = 4; // initial capacity
    node->labels = malloc(sizeof(char) * node->capacity);
    node->children = malloc(sizeof(TrieNode*) * node->capacity);
    node->suffixIndex = -1;
    return node;
}

// Return the child of `node` corresponding to character `c`, or NULL if not found
TrieNode *find_child(TrieNode *node, char c) {
    for (int i = 0; i < node->childCount; i++) {
        if (node->labels[i] == c)
            return node->children[i];
    }
    return NULL;
}

// Add a new child labeled by character `c` to `node`
TrieNode *add_child(TrieNode *node, char c, TrieNode *child) {
    if (node->childCount == node->capacity) {
        node->capacity *= 2;
        node->labels = realloc(node->labels, sizeof(char) * node->capacity);
        node->children = realloc(node->children, sizeof(TrieNode*) * node->capacity);
    }
    node->labels[node->childCount] = c;
    node->children[node->childCount] = child;
    node->childCount++;
    return child;
}

//
// Suffix Trie Construction and Destruction
//

// Build a suffix trie for the given text
SuffixTrie *st_create(char *text) {
    SuffixTrie *st = malloc(sizeof(SuffixTrie));
    st->text = strdup(text);
    st->size = strlen(text);
    st->root = new_trie_node();

    // Insert all suffixes into the trie
    for (int i = 0; i < st->size; i++) {
        TrieNode *curr = st->root;
        for (int j = i; j < st->size; j++) {
            char c = text[j];
            TrieNode *child = find_child(curr, c);
            if (!child) {
                child = new_trie_node();
                add_child(curr, c, child);
            }
            curr = child;
        }
        curr->suffixIndex = i;
    }

    return st;
}

// Free a trie node recursively
void st_free_node(TrieNode *node) {
    if (!node) return;
    for (int i = 0; i < node->childCount; i++) {
        st_free_node(node->children[i]);
    }
    free(node->labels);
    free(node->children);
    free(node);
}

// Free the entire suffix trie
void st_destroy(SuffixTrie *st) {
    st_free_node(st->root);
    free(st->text);
    free(st);
}

//
// Visualization
//

// Recursively print the trie in a compressed view
void st_print_node_compressed(TrieNode *node, char *text, char *prefix, int isLast, char edgeChar) {
    if (!node) return;

    // Print prefix and connector
    printf("%s", prefix);
    printf(isLast ? "└──" : "├──");

    // Build compressed path label
    char buffer[1024];
    int len = 0;
    buffer[len++] = edgeChar;

    TrieNode *curr = node;
    while (curr->childCount == 1 && curr->suffixIndex == -1) {
        buffer[len++] = curr->labels[0];
        curr = curr->children[0];
    }
    buffer[len] = '\0';

    // Print edge label and suffix index if leaf
    printf("%s", buffer);
    if (curr->suffixIndex != -1)
        printf(" [$] (%d)", curr->suffixIndex);
    printf("\n");

    // Prepare prefix for children
    char nextPrefix[1024];
    snprintf(nextPrefix, sizeof(nextPrefix), "%s%s", prefix, isLast ? "    " : "│   ");

    // Print children
    for (int i = 0; i < curr->childCount; i++) {
        st_print_node_compressed(curr->children[i], text, nextPrefix, i == curr->childCount - 1, curr->labels[i]);
    }
}

// Print the entire compressed suffix trie
void st_print_compressed(SuffixTrie *st) {
    printf("Compressed Suffix Trie View:\n");
    for (int i = 0; i < st->root->childCount; i++) {
        st_print_node_compressed(st->root->children[i], st->text, "", i == st->root->childCount - 1, st->root->labels[i]);
    }
}
