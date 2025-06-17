#include "suffix_trie.h"

TrieNode *new_trie_node() {
    TrieNode *node = malloc(sizeof(TrieNode));
    node->numChildren = 0;
    node->capacity = 4; // initial
    node->labels = malloc(sizeof(char) * node->capacity);
    node->children = malloc(sizeof(TrieNode*) * node->capacity);
    node->suffixIndex = -1;
    return node;
}

TrieNode *find_child(TrieNode *node, char c) {
    for (int i = 0; i < node->numChildren; i++) {
        if (node->labels[i] == c)
            return node->children[i];
    }
    return NULL;
}

TrieNode *add_child(TrieNode *node, char c, TrieNode *child) {
    if (node->numChildren == node->capacity) {
        node->capacity *= 2;
        node->labels = realloc(node->labels, sizeof(char) * node->capacity);
        node->children = realloc(node->children, sizeof(TrieNode*) * node->capacity);
    }
    node->labels[node->numChildren] = c;
    node->children[node->numChildren] = child;
    node->numChildren++;
    return child;
}

SuffixTrie *st_create(char *text) {
    SuffixTrie *st = malloc(sizeof(SuffixTrie));
    st->text = strdup(text);
    st->size = strlen(text);
    st->root = new_trie_node();

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

void st_free_node(TrieNode *node) {
    if (!node) return;
    for (int i = 0; i < node->numChildren; i++) {
        st_free_node(node->children[i]);
    }
    free(node->labels);
    free(node->children);
    free(node);
}

void st_destroy(SuffixTrie *st) {
    st_free_node(st->root);
    free((void*)st->text);
    free(st);
}
void st_print_node_compressed(TrieNode *node, char *text, char *prefix, int isLast, char edgeChar) {
    if (!node) return;

    // Préparer indentation
    printf("%s", prefix);
    printf(isLast ? "└──" : "├──");

    // Construire le chemin compressé
    char buffer[1024];
    int len = 0;
    buffer[len++] = edgeChar;

    TrieNode *curr = node;
    while (curr->numChildren == 1 && curr->suffixIndex == -1) {
        buffer[len++] = curr->labels[0];
        curr = curr->children[0];
    }
    buffer[len] = '\0';

    printf("%s", buffer);
    if (curr->suffixIndex != -1)
        printf(" [$] (%d)", curr->suffixIndex);
    printf("\n");

    // Nouvelle indentation
    char nextPrefix[1024];
    snprintf(nextPrefix, sizeof(nextPrefix), "%s%s", prefix, isLast ? "    " : "│   ");

    for (int i = 0; i < curr->numChildren; i++) {
        st_print_node_compressed(curr->children[i], text, nextPrefix, i == curr->numChildren - 1, curr->labels[i]);
    }
}

void st_print_compressed(SuffixTrie *st) {
    printf("Compressed Suffix Trie View:\n");
    for (int i = 0; i < st->root->numChildren; i++) {
        st_print_node_compressed(st->root->children[i], st->text, "", i == st->root->numChildren - 1, st->root->labels[i]);
    }
}