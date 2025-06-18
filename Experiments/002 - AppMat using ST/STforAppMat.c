#include "STforAppMat.h"


MatchList* createMatchList() {
    MatchList* list = malloc(sizeof(MatchList));
    list->size = 0;
    list->capacity = 16;
    list->data = malloc(sizeof(Match) * list->capacity);
    return list;
}

void freeOpArray(opArray o) {
    if (o.operations) free(o.operations);
}

opArray copyOpArray(opArray o) {
    opArray newO;
    newO.nbOp = o.nbOp;
    newO.operations = malloc(sizeof(operation) * (o.nbOp + 100));
    memcpy(newO.operations, o.operations, sizeof(operation) * o.nbOp);
    return newO;
}

void addOrUpdateMatch(MatchList* list, int pos, int nbModifs, opArray ops) {
    for (int i = 0; i < list->size; i++) {
        if (list->data[i].endPosition == pos) {
            if (nbModifs < list->data[i].nbModifs) {
                freeOpArray(list->data[i].ops);
                list->data[i].nbModifs = nbModifs;
                list->data[i].ops = copyOpArray(ops);
            }
            return;
        }
    }

    if (list->size >= list->capacity) {
        list->capacity *= 2;
        list->data = realloc(list->data, sizeof(Match) * list->capacity);
    }

    list->data[list->size++] = (Match){pos, nbModifs, copyOpArray(ops)};
}

int cmpMatch(const void* a, const void* b) {
    return ((Match*)a)->endPosition - ((Match*)b)->endPosition;
}

void printMatchList(MatchList* list, char* text, char* pattern) {
    qsort(list->data, list->size, sizeof(Match), cmpMatch);
    for (int idx = 0; idx < list->size; idx++) {
        Match m = list->data[idx];
        printf("✅ Match at position %d with %d edits.\n", m.endPosition, m.nbModifs);

        int nbOp = m.ops.nbOp;
        char t[nbOp + 1], p[nbOp + 1], op[nbOp + 1];
        int j = 1, k = 0;

        for (int i = nbOp - 1; i >= 0; i--) {
            switch (m.ops.operations[i]) {
                case INS:  t[i] = text[m.endPosition - k]; p[i] = '-'; op[i] = ' '; k++; break;
                case DEL:  t[i] = '-'; p[i] = pattern[strlen(pattern) - j]; op[i] = ' '; j++; break;
                case SUB:  t[i] = text[m.endPosition - k]; p[i] = pattern[strlen(pattern) - j]; op[i] = '*'; j++; k++; break;
                case EQ:   t[i] = text[m.endPosition - k]; p[i] = pattern[strlen(pattern) - j]; op[i] = '|'; j++; k++; break;
            }
        }
        t[nbOp] = p[nbOp] = op[nbOp] = '\0';
        printf("%s\n%s\n%s\n\n", t, op, p);
    }
}

void freeMatchList(MatchList* list) {
    for (int i = 0; i < list->size; i++) {
        freeOpArray(list->data[i].ops);
    }
    free(list->data);
    free(list);
}

//
// Distance Array
//

distArray initArray(int len) {
    distArray d;
    d.array = malloc(sizeof(int) * (len + 1));
    d.op = malloc(sizeof(opArray) * (len + 1));
    d.len = len;

    for (int i = 0; i <= len; i++) {
        d.array[i] = i;
        d.op[i].nbOp = i;
        d.op[i].operations = malloc(sizeof(operation) * (len + 1));
        for (int j = 0; j < i; j++) d.op[i].operations[j] = DEL;
    }

    return d;
}

distArray copyDistArray(distArray d) {
    distArray newD = initArray(d.len);
    for (int i = 0; i <= d.len; i++) {
        newD.array[i] = d.array[i];
        freeOpArray(newD.op[i]);
        newD.op[i] = copyOpArray(d.op[i]);
    }
    return newD;
}



void majDistArray(char c, char* pattern, distArray d) {
    int len = d.len;
    int* prevArray = malloc(sizeof(int) * (len + 1));
    memcpy(prevArray, d.array, sizeof(int) * (len + 1));

    opArray* prevOp = malloc(sizeof(opArray) * (len + 1));
    for (int i = 0; i <= len; i++) prevOp[i] = copyOpArray(d.op[i]);

    d.array[0] = prevArray[0] + 1;
    freeOpArray(d.op[0]);
    d.op[0] = copyOpArray(prevOp[0]);
    d.op[0].operations[d.op[0].nbOp++] = INS;

    for (int i = 1; i <= len; i++) {
        int cost = (pattern[i - 1] != c);
        int del = d.array[i - 1] + 1;
        int ins = prevArray[i] + 1;
        int subeq = prevArray[i - 1] + cost;

        int min = ins;
        opArray chosen = copyOpArray(prevOp[i]);
        chosen.operations[chosen.nbOp++] = INS;

        if (del < min) {
            min = del;
            freeOpArray(chosen);
            chosen = copyOpArray(d.op[i - 1]);
            chosen.operations[chosen.nbOp++] = DEL;
        }

        if (subeq < min) {
            min = subeq;
            freeOpArray(chosen);
            chosen = copyOpArray(prevOp[i - 1]);
            chosen.operations[chosen.nbOp++] = cost ? SUB : EQ;
        }

        d.array[i] = min;
        freeOpArray(d.op[i]);
        d.op[i] = chosen;
    }

    free(prevArray);
    for (int i = 0; i <= len; i++) freeOpArray(prevOp[i]);
    free(prevOp);
}

void freeDistArray(distArray* d) {
    if (!d) return;
    if (d->array) free(d->array);
    if (d->op) {
        for (int i = 0; i <= d->len; i++) freeOpArray(d->op[i]);
        free(d->op);
    }
}

//
// Approximate Matching
//

int getStartPositionOpArray(opArray ops, int endPosition) {
    int start = endPosition + 1; // Start after the last character of the match
    for (int i = ops.nbOp - 1; i >= 0; i--) {
        switch (ops.operations[i]) {
            case DEL: break;
            default: // INS, SUB, EQ
                start--;
                break;
        }
    }
    return start;
}

int get_match_position(TrieNode* node) {
    while (node && node->childCount == 1 && node->suffixIndex == -1)
        node = node->children[0];
    return (node && node->suffixIndex != -1) ? node->suffixIndex : -1;
}

void dfs_approximate_matching(TrieNode* node, char* text, char* pattern, int k, distArray d, int depth, MatchList* list) {
    if (!node) return;

    int viable = 0;
    for (int i = 0; i <= d.len; i++) {
        if (d.array[i] <= k) {
            viable = 1;
            break;
        }
    }
    if (!viable) return;

    for (int i = 0; i < node->childCount; i++) {
        char c = node->labels[i];
        distArray newD = copyDistArray(d);
        majDistArray(c, pattern, newD);

        if (newD.array[newD.len] <= k) {
            int matchPos = get_match_position(node);
            if (matchPos >= 0) {
                addOrUpdateMatch(list, matchPos + depth, newD.array[newD.len], newD.op[newD.len]);
            }
        }

        dfs_approximate_matching(node->children[i], text, pattern, k, newD, depth + 1, list);
        freeDistArray(&newD);
    }
}

void approximateMatching(SuffixTrie* st, char* pattern, int k, MatchList* list) {
    distArray d = initArray(strlen(pattern));

    dfs_approximate_matching(st->root, st->text, pattern, k, d, 0, list);
    freeDistArray(&d);
}