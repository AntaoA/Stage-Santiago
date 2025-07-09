#ifndef E618C4AC_DEA8_47C6_9C61_F8B9D5C07DA5
#define E618C4AC_DEA8_47C6_9C61_F8B9D5C07DA5


int rank1(const unsigned int* FT, int n);

int select1(const unsigned int* FT, int n, int rank);

int pred1(const unsigned int* FT, int n, int pos);

int select_c(const unsigned char* FT, int len, unsigned char c, int n_c);

int rank0(const unsigned int* FT, int n);

int rank_c(const unsigned char* FT, int n, unsigned char c);

int letter_conv(unsigned char c);

int max(unsigned int a, unsigned int b);

#endif /* E618C4AC_DEA8_47C6_9C61_F8B9D5C07DA5 */
