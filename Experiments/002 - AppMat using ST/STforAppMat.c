// approximate matching algorithm using suffix tree
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#include "suffix_tree.h"


//type bool avec  3 valeurs
typedef enum { INS, DEL, SUB, EQ } operation;

typedef struct {
    int nbOp; // nombre d'opérations
    operation* operations; // tableau des opérations
} opArray;

typedef struct {
    int* array; // tableau des distances
    opArray* op; // tableau des opérations
} distArray;

distArray majArray (char lettreAdd, char* pattern, int* array, opArray* op) {
    int len = strlen(pattern);
    int* newArray = malloc(sizeof(int) * (len + 1));
    opArray* newOp = malloc(sizeof(opArray) * (len + 1));
    for (int i = 0; i <= len; i++) {
        newOp[i].operations = malloc(sizeof(operation) * (len + 1));
    }

    newOp[0].nbOp = op[0].nbOp + 1;
    for (int i = 0; i <= op[0].nbOp; i++) {
        newOp[0].operations[i] = op[0].operations[i];
    }
    newOp[0].operations[op[0].nbOp] = INS;
    newArray[0] = array[0] + 1;


    for (int i = 1; i <= strlen(pattern); i++) {
        int val_i = array[i] + 1; // Insertion
        int val_d = newArray[i - 1] + 1; // Suppression
        int eq = (pattern[i - 1] != lettreAdd); // Égalité
        int val_s = array[i - 1] + eq; // Substitution

        if (val_s <= val_i && val_s <= val_d) { // Substitution
            newArray[i] = val_s;
            newOp[i].nbOp = op[i - 1].nbOp + 1;
            for (int j = 0; j <= op[i - 1].nbOp; j++) {
                newOp[i].operations[j] = op[i - 1].operations[j];
            }
            if (eq) {
                newOp[i].operations[op[i - 1].nbOp] = SUB;
            } else {
                newOp[i].operations[op[i - 1].nbOp] = EQ;
            }
        } else if (val_i <= val_d && val_i <= val_s) { // Insertion
            newArray[i] = val_i;
            newOp[i].nbOp = op[0].nbOp + 1;
            for (int j = 0; j <= op[0].nbOp; j++) {
                newOp[i].operations[j] = op[0].operations[j];
            }
            newOp[i].operations[op[0].nbOp] = INS;
        } else { // Suppression
            newArray[i] = val_d;
            newOp[i].nbOp = op[0].nbOp + 1;
            for (int j = 0; j <= op[0].nbOp; j++) {
                newOp[i].operations[j] = op[0].operations[j];
            }
            newOp[i].operations[op[0].nbOp] = DEL;
        }
    }
    return (distArray){newArray, newOp};
}


void freeDistArray(distArray* d) {
    if (d == NULL) return;
    if (d->array != NULL) free(d->array);
    if (d->op != NULL) {
        for (int i = 0; i <= strlen(d->op[0].operations); i++) {
            free(d->op[i].operations);
        }
        free(d->op);
    }
    free(d);
}

distArray* findAppMat(SuffixTree* st, char* pattern) {
    int len = strlen(pattern);
    int* array = malloc(sizeof(int) * (len + 1));
    opArray* op = malloc(sizeof(opArray) * (len + 1));

    for (int i = 0; i <= len; i++) {
        array[i] = i; // Initialisation de la première ligne
        op[i].nbOp = 0;
        op[i].operations = malloc(sizeof(operation) * (len + 1));

    }
    for (int i = 1; i <= len; i++) {
        op[i].operations[0] = DEL;
    }

    
}

int main() {
    char* text = "banana$";
    SuffixTree *st = st_create(text);
    st_print(st);
    st_destroy(st);
    return 0;
}