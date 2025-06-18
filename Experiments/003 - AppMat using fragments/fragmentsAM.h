#ifndef CBDC2D85_F9CE_4DC6_985C_59517E577DA2
#define CBDC2D85_F9CE_4DC6_985C_59517E577DA2

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// === Utilities ===

#define MIN(a,b) ((a)<(b)?(a):(b))
#define MAX(a,b) ((a)>(b)?(a):(b))
#define INF 999999

// === Operation Enum for Edit Distance ===
typedef enum { D, I, S, E } Operation;

// === Alignment Struct to Store Edit Info ===
typedef struct {
    int k; // edit distance
    int nbOp; // number of operations
    Operation* operations; // list of operations
} Alignment;

// === Fragment Representing a Substring of Pattern ===
typedef struct fragment {
    int id;
    int start;
    int end;
    char* text;
} Fragment;

// === List of Fragments ===
typedef struct fragment_list {
    Fragment* fragment;
    int size;
    int capacity;
} FragmentsList;

// === Candidate Match in Text for a Fragment ===
typedef struct {
    Fragment fragment;
    int start_text;
    int end_text;
} Candidate;

// === List of Candidates ===
typedef struct candidate_list {
    Candidate* candidate;
    int size;
    int capacity;
} CandidatesList;




// === Function Prototypes ===

FragmentsList* create_fragment_list();
CandidatesList* create_candidate_list();
Candidate copy_candidate(Candidate* src);
void free_alignment(Alignment* alignment);
void free_fragment_list(FragmentsList* list);
void free_candidate_list(CandidatesList* list);
void print_fragment_list(FragmentsList* list);
void print_candidate_list(CandidatesList* list);
void strrev(char *str);
FragmentsList* make_fragments(char* pattern, int k);
CandidatesList* exact_match(char* text, Fragment frag);
void add_candidates(CandidatesList* dest, CandidatesList* src);
CandidatesList* find_candidates(char* text, char* pattern, int k);
Alignment* edit_distance_aligned(char* text, char* pattern);
void verify_candidates(CandidatesList* candidates_list, char* text, char* pattern, int k);
void revOperations(Alignment* align);
void append_char(char* s, char c);
int getStartPositionAlignment(Alignment* align, int endPosition);

#endif /* CBDC2D85_F9CE_4DC6_985C_59517E577DA2 */
