#ifndef E2C0A67F_113F_4E72_9106_3FF128C364E8
#define E2C0A67F_113F_4E72_9106_3FF128C364E8

#include "sa.h"
#include "lsa.h"

// T : texte d'entrée (terminé par '$')
// n : longueur du texte
// SA : Suffix Array de taille n, calculé par build_suffix_array
// BWT : tableau de taille n alloué par l'appelant
// return : position du symbole T[n-1] dans la BWT (le '$')

int build_bwt(const unsigned char* T, int n, int* SA, unsigned char* BWT);

unsigned int* start(BWT_Run* runs, int n, int num_runs);
unsigned char* letter(BWT_Run* runs, int num_runs);

#endif /* E2C0A67F_113F_4E72_9106_3FF128C364E8 */
