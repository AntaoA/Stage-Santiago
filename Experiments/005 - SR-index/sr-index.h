#ifndef B0119CA7_3AF5_44DC_BAB5_C282EC6ABFED
#define B0119CA7_3AF5_44DC_BAB5_C282EC6ABFED

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "lsa.h"
#include "sa.h"
#include "bwt.h"
#include "ft.h"
#include "mapfl.h"
#include "c.h"
#include "lf.h"

typedef struct srIndex {
    int s;
    const char* alphabet;
    int n_alphabet;
    int n;
    unsigned int* start;
    unsigned char* letters;
    int n_runs;
    LSA lsa;
    unsigned int* del;
    unsigned int* FT;
    unsigned int* mapFL;
    unsigned int* C;
    unsigned int* LF;
    unsigned int last_sa;
} srIndex;

srIndex* create_sr_index(const char* text, int s);

#endif /* B0119CA7_3AF5_44DC_BAB5_C282EC6ABFED */
