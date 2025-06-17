// approximate matching algorithm using suffix tree
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "suffix_trie.h"



typedef enum { INS, DEL, SUB, EQ } operation;

typedef struct {
    int nbOp; // nombre d'opérations
    operation* operations; // tableau des opérations
} opArray;

typedef struct {
    int* array; // tableau des distances
    opArray* op; // tableau des opérations
    int len; // longueur du motif
} distArray;

typedef struct {
    int positionFinale;
    int nbModifs;
    opArray ops;
} Match;

typedef struct {
    Match* data;
    int size;
    int capacity;
} MatchList;


MatchList* createMatchList() {
    MatchList* list = malloc(sizeof(MatchList));
    list->size = 0;
    list->capacity = 16;
    list->data = malloc(sizeof(Match) * list->capacity);
    return list;
}


void freeOpArray(opArray o) {
    if (o.operations != NULL) {
        free(o.operations);
    }
}

opArray copyOpArray(opArray o) {
    opArray newO;
    newO.nbOp = o.nbOp;
    newO.operations = malloc(sizeof(operation) * (o.nbOp + 100));
    for (int i = 0; i < o.nbOp; i++) {
        newO.operations[i] = o.operations[i];
    }
    return newO;
}

void addOrUpdateMatch(MatchList* list, int pos, int nbModifs, opArray ops) {
    // Si déjà un match à cette position, garder le meilleur
    for (int i = 0; i < list->size; i++) {
        if (list->data[i].positionFinale == pos) {
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

    list->data[list->size].positionFinale = pos;
    list->data[list->size].nbModifs = nbModifs;
    list->data[list->size].ops = copyOpArray(ops);
    list->size++;
}


int cmpMatch(const void* a, const void* b) {
    return ((Match*)a)->positionFinale - ((Match*)b)->positionFinale;
}

void printMatchList(MatchList* list, char* text, char* pattern) {
    qsort(list->data, list->size, sizeof(Match), cmpMatch);

    for (int idx = 0; idx < list->size; idx++) {
        Match m = list->data[idx];
        printf("✅ Match trouvé à la position finale %d (à minima) avec %d modifications autorisées.\n", m.positionFinale, m.nbModifs);
        int nbOperations = m.ops.nbOp;
        char t[nbOperations + 1], p[nbOperations + 1], op[nbOperations + 1];
        int j = 1, k = 0;
        for (int i = nbOperations - 1; i >= 0; i--) {
            switch (m.ops.operations[i]) {
                case INS:
                    t[i] = text[m.positionFinale - k];
                    p[i] = '-';
                    op[i] = ' ';
                    k++;
                    break;
                case DEL:
                    t[i] = '-';
                    p[i] = pattern[strlen(pattern) - j];
                    op[i] = ' ';
                    j++;
                    break;
                case SUB:
                    t[i] = text[m.positionFinale - k];
                    p[i] = pattern[strlen(pattern) - j];
                    op[i] = '*';
                    j++; k++;
                    break;
                case EQ:
                    t[i] = text[m.positionFinale - k];
                    p[i] = pattern[strlen(pattern) - j];
                    op[i] = '|';
                    j++; k++;
                    break;
            }
        }
        t[nbOperations] = p[nbOperations] = op[nbOperations] = '\0';
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


distArray initArray(int len) {
    int* array = malloc(sizeof(int) * (len + 100));
    opArray* op = malloc(sizeof(opArray) * (len + 100));
    for (int i = 0; i <= len; i++) {
        op[i].operations = malloc(sizeof(operation) * (len + 100));
    }

    for (int i = 0; i <= len; i++) {
        array[i] = i; // Initialisation des distances
        op[i].nbOp = i; // Aucune opération au départ
        for (int j = 0; j < i; j++) {
            op[i].operations[j] = DEL; // Initialisation des opérations à égalité
        }
    }

    return (distArray){array, op, len};
}

distArray copyDistArray(distArray d) {
    distArray newD;
    newD.len = d.len;
    newD.array = malloc(sizeof(int) * (d.len + 100));
    newD.op = malloc(sizeof(opArray) * (d.len + 100));
    for (int i = 0; i <= d.len; i++) {
        newD.array[i] = d.array[i];
        newD.op[i].nbOp = d.op[i].nbOp;
        newD.op[i].operations = malloc(sizeof(operation) * (d.len + 100));
        for (int j = 0; j < d.op[i].nbOp; j++) {
            newD.op[i].operations[j] = d.op[i].operations[j];
        }
    }
    return newD;
}



void majDistArray(char lettreAdd, char* pattern, distArray d) {
    int len = d.len;

    // 1. Copier l'ancienne ligne de distance et d'opérations
    int* prevArray = malloc(sizeof(int) * (len + 1));
    for (int i = 0; i <= len; i++) {
        prevArray[i] = d.array[i];
    }

    opArray* prevOp = malloc(sizeof(opArray) * (len + 1));
    for (int i = 0; i <= len; i++) {
        prevOp[i] = copyOpArray(d.op[i]);
    }

    // 2. Mise à jour d[0] = insertion
    d.array[0] = prevArray[0] + 1;
    freeOpArray(d.op[0]);
    d.op[0] = copyOpArray(prevOp[0]);
    d.op[0].nbOp++;
    d.op[0].operations[d.op[0].nbOp - 1] = INS;

    // 3. Boucle de 1 à len
    for (int i = 1; i <= len; i++) {
        int costSub = (pattern[i - 1] != lettreAdd) ? 1 : 0;

        int del  = d.array[i - 1] + 1;
        int ins  = prevArray[i] + 1;
        int subeq = prevArray[i - 1] + costSub;

        int min = ins;
        opArray chosen = copyOpArray(prevOp[i]);
        chosen.nbOp++;
        chosen.operations[chosen.nbOp - 1] = INS;

        if (del < min) {
            min = del;
            freeOpArray(chosen);
            chosen = copyOpArray(d.op[i - 1]);
            chosen.nbOp++;
            chosen.operations[chosen.nbOp - 1] = DEL;
        }

        if (subeq < min) {
            min = subeq;
            freeOpArray(chosen);
            chosen = copyOpArray(prevOp[i - 1]);
            chosen.nbOp++;
            chosen.operations[chosen.nbOp - 1] = (costSub ? SUB : EQ);
        }

        d.array[i] = min;
        freeOpArray(d.op[i]);
        d.op[i] = chosen;
    }
    /*
    // 4. Debug print
    printf("\nLettre ajoutée: %c\n", lettreAdd);
    printf("Distances : ");
    for (int j = 0; j <= len; j++) printf("%d ", d.array[j]);
    printf("\n");
    */

    // 5. Libération
    free(prevArray);
    for (int i = 0; i <= len; i++) {
        freeOpArray(prevOp[i]);
    }
    free(prevOp);
}


void printDistArray(distArray d) {
    int len = d.len;
    printf("Distance array:\n");
    for (int i = 0; i <= len; i++) {
        printf("%d ", d.array[i]);
    }
    printf("\nOperations:\n");
    for (int i = 0; i <= len; i++) {
        printf("Index %d: ", i);
        for (int j = 0; j < d.op[i].nbOp; j++) {
            switch (d.op[i].operations[j]) {
                case INS: printf("INS "); break;
                case DEL: printf("DEL "); break;
                case SUB: printf("SUB "); break;
                case EQ:  printf("EQ "); break;
            }
        }
        printf("\n");
    }
}


void freeDistArray(distArray* d) {
    if (d == NULL) return;
    if (d->array != NULL) free(d->array);
    if (d->op != NULL) {
        for (int i = 0; i <= d->len; i++) {
            freeOpArray(d->op[i]);
        }
        free(d->op);
    }
}

int get_match_position(TrieNode* node) {
    while (node != NULL && node->numChildren == 1 && node->suffixIndex == -1) {
        node = node->children[0]; // Descendre dans l'arbre jusqu'à un noeud terminal ou avec plusieurs enfants
    }
    if (node == NULL) return -1; // Aucun match trouvé
    if (node->suffixIndex != -1) {
        return node->suffixIndex + 1;
    }
    return -1;
    
}



void dfs_approximate_matching(TrieNode* node, char* text, char* pattern, int k, distArray d, int textDepth, MatchList* matchList) {
    if (node == NULL) return;

    int viable = 0;
    for (int i = 0; i <= d.len; i++) {
        if (d.array[i] <= k) {
            viable = 1;
            break;
        }
    }
    if (!viable) return;

    // 🔁 Exploration récursive sur les enfants
    for (int i = 0; i < node->numChildren; i++) {
        char lettreAdd = node->labels[i]; // caractère sur l'arête
        distArray newD = copyDistArray(d);
        majDistArray(lettreAdd, pattern, newD);
        if (newD.array[newD.len] <= k) {
            int matchPos = get_match_position(node);
            if (matchPos >= 0) {
                addOrUpdateMatch(matchList, matchPos + textDepth, newD.array[newD.len], newD.op[newD.len]);
            }
        }
        // 📦 Propagation avec une profondeur textuelle accrue
        dfs_approximate_matching(node->children[i], text, pattern, k, newD, textDepth + 1, matchList);
        freeDistArray(&newD);
    }
}


void approximateMatching(SuffixTrie* st, char* pattern, int k) {
    // Implémentation de l'algorithme de correspondance approximative
    // Utilisation de la structure distArray pour stocker les distances
    int len = strlen(pattern);
    distArray d = initArray(len);
    MatchList* matchList = createMatchList();
    dfs_approximate_matching(st->root, st->text, pattern, k, d, 0, matchList);
    printMatchList(matchList, st->text, pattern);
    freeDistArray(&d);
}



int main() {
    char text[100], pattern[100];
    printf("Enter the text: ");
    scanf("%s", text);
    printf("Enter the pattern: ");
    scanf("%s", pattern)
    ;
    SuffixTrie* st = st_create(text);

    st_print_compressed(st);

    int k;
    printf("\nEnter the maximum number of errors allowed: ");
    scanf("%d", &k);

    approximateMatching(st, pattern, k);
    st_destroy(st);
    return 0;
}