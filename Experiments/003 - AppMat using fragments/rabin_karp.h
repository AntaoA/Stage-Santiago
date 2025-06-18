#ifndef DA6C0059_9881_4FCD_B14E_521FC6C851DE
#define DA6C0059_9881_4FCD_B14E_521FC6C851DE

#include "fragmentsAM.h"


#define BASE1 33
#define PRIME1 1000003
#define BASE2 37
#define PRIME2 1000033

// === Hash Pair for Rabin-Karp ===
typedef struct {
    unsigned long long h1;
    unsigned long long h2;
} HashPair;


void rabin_karp(char* text, Fragment frag, CandidatesList* list);



#endif /* DA6C0059_9881_4FCD_B14E_521FC6C851DE */
