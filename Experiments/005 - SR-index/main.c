
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "sr-index.h"

int main() {
    const char* input = "ALABARALAALABARDA$";
    int s = 2; // Paramètre de subsampling
    srIndex* index = create_sr_index(input, s);
    if (index == NULL) {
        fprintf(stderr, "Erreur lors de la création de l'index.\n");
        return EXIT_FAILURE;
    }
    printf("Index créé avec succès.\n");

    // Libération de la mémoire allouée
    free(index->start);
    free(index->letters);
    free(index->lsa.LSA);
    free(index->del);
    free(index->FT);
    free(index->mapFL);
    free(index->C);
    free(index->LF);
    free(index);
    return EXIT_SUCCESS;
}
