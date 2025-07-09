#include "../005 - SR-index/sr-index.h"
#include "count_approx.h"


int main() {
    const char* input = "ALABARALAALABARDA$";
    int s = 2; // Subsampling parameter
    srIndex* index = create_sr_index(input, s);
    if (index == NULL) {
        fprintf(stderr, "Error creating the index.\n");
        return EXIT_FAILURE;
    }
    printf("Index created successfully.\n");


    char pattern[] = "ARBA";
    int k = 1; // Maximum allowed errors
    printf("Counting approximate occurrences of pattern '%s' with up to %d errors...\n", pattern, k);
    count_approx(index, pattern, k); 




    printf("Tableau de FindToehold:\n");
    for (int p = 1; p < 11; p++) {
        for (int i = 1; i < 5; i++) {
            int v = findToehold_approx(index, pattern, i, p);
            printf("%d ", v);
        }
        printf("\n");
    }

    // Free allocated memory
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