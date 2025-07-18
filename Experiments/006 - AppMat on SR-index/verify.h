#ifndef EB2771B5_2920_4967_B235_0C127246FA6E
#define EB2771B5_2920_4967_B235_0C127246FA6E

#include "../005 - SR-index/sr-index.h"
#include "count_approx.h"

typedef struct {
    int err_max;
    int position_start;
} verification_result;

verification_result verify_left(srIndex* index, int position, int position_sa, int k_left_max, const char* pattern, int id_left);

int verify_right(srIndex* index, int position, int position_sa, int k_right_max, const char* pattern, int id_right, int nb_letters_seen_in_text);



#endif /* EB2771B5_2920_4967_B235_0C127246FA6E */
