#include "STforAppMat.h"


int main() {
    char text[100], pattern[100];
    printf("Enter the text: ");
    scanf("%s", text);
    printf("Enter the pattern: ");
    scanf("%s", pattern);

    SuffixTrie* st = st_create(text);
    st_print_compressed(st);

    int k;
    printf("\nMaximum allowed errors: ");
    scanf("%d", &k);
    MatchList* matches = createMatchList();
    approximateMatching(st, pattern, k, matches);
    printMatchList(matches, st->text, pattern);
    freeMatchList(matches);
    st_destroy(st);
    return 0;
}
