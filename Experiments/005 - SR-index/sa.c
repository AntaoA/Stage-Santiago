#include "sa.h"

void build_suffix_array(const unsigned char* T, int n, int* SA) {
    divsufsort(T, SA, n);
}
