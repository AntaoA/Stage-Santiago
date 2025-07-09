#ifndef CDC88262_36E2_4EF9_A11F_BA833C8C8224
#define CDC88262_36E2_4EF9_A11F_BA833C8C8224
#include "findToehold.h"

typedef struct {
    int sp;
    int ep;
    int v;
} plage;

plage count(const char *P, int n, int m, 
           const unsigned int *FT, int *SA,
           const unsigned char *letters, const unsigned int *starts,
           const unsigned int *del, const unsigned int *LF, const unsigned int *LSA,
           const unsigned int *C, const unsigned char *BWT);
           
#endif /* CDC88262_36E2_4EF9_A11F_BA833C8C8224 */
