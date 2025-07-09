#ifndef CAF22D8F_EFC8_4A79_A55F_B883480B541F
#define CAF22D8F_EFC8_4A79_A55F_B883480B541F

#include <stdlib.h>
#include "utils.h"

// lf(i) = C[c] + rank_c(BWT, i, c) where c = BWT[i]

unsigned int* build_lf(unsigned int* C, unsigned char* BWT, int n);

#endif /* CAF22D8F_EFC8_4A79_A55F_B883480B541F */
