#ifndef D5754C51_8C0D_4EB9_8DF5_BDC55FDC91A4
#define D5754C51_8C0D_4EB9_8DF5_BDC55FDC91A4

#include <divsufsort.h>


// T : texte d'entrée (terminé par '$')
// n : longueur du texte (y compris '$')
// SA : tableau de taille n, alloué par l'appelant
void build_suffix_array(const unsigned char* T, int n, int* SA);


#endif /* D5754C51_8C0D_4EB9_8DF5_BDC55FDC91A4 */
