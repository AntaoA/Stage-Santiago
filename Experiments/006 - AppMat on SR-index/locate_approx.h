#ifndef D7F425FA_1D5F_444F_8F35_3926AC7AE95C
#define D7F425FA_1D5F_444F_8F35_3926AC7AE95C

#include "../005 - SR-index/sr-index.h"
#include "../005 - SR-index/utils.h"
#include "count_approx.h"


typedef struct {
    int position;
    int errors;
    int position_sa;
    int nb_letters_seen_in_text;
} match;

typedef struct {
    char* pattern;
    match* matches;
    int max_errors;
    int match_count;
} approx_matching;

approx_matching* locate_approx(srIndex* index, char* pattern, int k);

#endif /* D7F425FA_1D5F_444F_8F35_3926AC7AE95C */
