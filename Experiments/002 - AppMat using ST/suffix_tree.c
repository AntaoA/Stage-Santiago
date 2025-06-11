#include "suffix_tree.h"

// -------------------- Per-file globals --------------------
static SuffixTree *g_st;        // pour free_node
static int tree_size;           // longueur du texte

// Helper: allocate a new suffix tree node
static SuffixNode *new_node(int start, int *end) {
    SuffixNode *node = malloc(sizeof(SuffixNode));
    for (int i = 0; i < MAX_CHAR; i++) node->children[i] = NULL;
    node->suffixLink = NULL;
    node->start = start;
    node->end = end;
    node->suffixIndex = -1;
    return node;
}

// Compute edge length
static int edge_length(SuffixNode *n) {
    return *(n->end) - n->start + 1;
}

// Attempt to walk down active point
static int walk_down(SuffixTree *st, SuffixNode *curr) {
    if (st->activeLength >= edge_length(curr)) {
        st->activeEdge += edge_length(curr);
        st->activeLength -= edge_length(curr);
        st->activeNode = curr;
        return 1;
    }
    return 0;
}

// Extension step of Ukkonen
void extendSuffixTree(SuffixTree *st, int pos) {
    g_st = st; // pour free_node
    st->leafEnd = pos;
    st->remainingSuffixCount++;
    st->lastNewNode = NULL;

    while (st->remainingSuffixCount > 0) {
        if (st->activeLength == 0)
            st->activeEdge = pos;
        int idx = (unsigned char)st->text[st->activeEdge];
        if (!st->activeNode->children[idx]) {
            // création feuille
            st->activeNode->children[idx] = new_node(pos, &st->leafEnd);
            if (st->lastNewNode)
                st->lastNewNode->suffixLink = st->activeNode;
            st->lastNewNode = NULL;
        } else {
            SuffixNode *next = st->activeNode->children[idx];
            if (walk_down(st, next)) continue;
            char c = st->text[pos];
            char edgeChar = st->text[next->start + st->activeLength];
            if (edgeChar == c) {
                if (st->lastNewNode && st->activeNode != st->root) {
                    st->lastNewNode->suffixLink = st->activeNode;
                    st->lastNewNode = NULL;
                }
                st->activeLength++;
                break;
            }
            // Split de l'arête
            int *splitEnd = malloc(sizeof(int));
            *splitEnd = next->start + st->activeLength - 1;
            SuffixNode *split = new_node(next->start, splitEnd);
            st->activeNode->children[idx] = split;
            // nouvelle feuille
            split->children[(unsigned char)c] = new_node(pos, &st->leafEnd);
            // ajuster next
            next->start += st->activeLength;
            split->children[(unsigned char)st->text[next->start]] = next;
            if (st->lastNewNode)
                st->lastNewNode->suffixLink = split;
            st->lastNewNode = split;
        }
        st->remainingSuffixCount--;
        if (st->activeNode == st->root && st->activeLength > 0) {
            st->activeLength--;
            st->activeEdge = pos - st->remainingSuffixCount + 1;
        } else if (st->activeNode != st->root) {
            st->activeNode = st->activeNode->suffixLink;
        }
    }
}

// DFS to assign suffixIndex to leaves
static void set_suffix_index(SuffixTree *st, SuffixNode *n, int height) {
    if (!n) return;
    int leaf = 1;
    for (int i = 0; i < MAX_CHAR; i++) {
        if (n->children[i]) {
            leaf = 0;
            set_suffix_index(st, n->children[i], height + edge_length(n->children[i]));
        }
    }
    if (leaf) {
        n->suffixIndex = tree_size - height;
    }
}

// Recursively free nodes
static void free_node(SuffixNode *n) {
    if (!n) return;
    for (int i = 0; i < MAX_CHAR; i++)
        free_node(n->children[i]);
    // libérer only allocated ends
    if (n->end && n->end != &n->start && n->end != &g_st->leafEnd && n->end != g_st->rootEnd)
        free(n->end);
    free(n);
}

// Build a suffix tree for text ending with '$'
SuffixTree *st_create(const char *text) {
    SuffixTree *st = malloc(sizeof(SuffixTree));
    st->text = strdup(text);
    st->size = strlen(text);
    tree_size = st->size;
    st->rootEnd = malloc(sizeof(int));
    *st->rootEnd = -1;
    st->root = new_node(-1, st->rootEnd);
    st->activeNode = st->root;
    st->activeEdge = -1;
    st->activeLength = 0;
    st->remainingSuffixCount = 0;
    st->leafEnd = -1;
    st->lastNewNode = NULL;
    for (int i = 0; i < st->size; i++)
        extendSuffixTree(st, i);
    set_suffix_index(st, st->root, 0);
    return st;
}

// Print the tree recursively
static void _printNode(SuffixTree *st, SuffixNode *n, int depth) {
    if (!n) return;
    for (int c = 0; c < MAX_CHAR; c++) {
        SuffixNode *child = n->children[c];
        if (child) {
            for (int i = 0; i < depth; i++) printf("  ");
            int len = edge_length(child);
            printf("%.*s [%d]\n", len, st->text + child->start, child->suffixIndex);
            _printNode(st, child, depth + 1);
        }
    }
}

void st_print(SuffixTree *st) {
    printf("Suffix Tree:\n");
    _printNode(st, st->root, 0);
}

// Destroy the tree
void st_destroy(SuffixTree *st) {
    free_node(st->root);
    free(st->rootEnd);
    free(st->text);
    free(st);
}
