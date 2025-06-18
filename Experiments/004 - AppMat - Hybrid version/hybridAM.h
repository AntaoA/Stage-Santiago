#ifndef BD98D5D6_0618_4155_871E_18C933D1A4A5
#define BD98D5D6_0618_4155_871E_18C933D1A4A5

#include "../002 - AppMat using ST/STforAppMat.h"
#include "../003 - AppMat using fragments/fragmentsAM.h"
#include <string.h>

/// Structure that binds a MatchList with the corresponding pattern string
typedef struct {
    MatchList* matchList;  // list of approximate matches
    char* pattern;         // corresponding pattern
} MatchListWithPattern;

/// Array of match lists, one per pattern
typedef struct {
    MatchListWithPattern* data; // array of match lists per pattern
    int size;                   // number of patterns
} MatchListArray;

/// Structure combining candidate fragments and their edit alignments
typedef struct {
    CandidatesList* candidates;  // candidate fragment list
    Alignment* align;            // corresponding alignments
} CandidatesWithErrors;

/// Frees memory allocated for CandidatesWithErrors
void free_candidates_with_errors(CandidatesWithErrors* candidates);

/// Creates an array of match lists, each associated with a pattern
MatchListArray create_match_list_array(int k);

/// Frees a MatchListArray and its associated resources
void free_match_list_array(MatchListArray array);

/// Prints all match lists for several patterns in a readable format
void print_match_list_multiple_patterns(MatchListArray matches, char* text);

/// Creates a deep copy of a MatchList
MatchList* copy_match_list(MatchList* src);

/// Runs the hybrid partial matching algorithm using ST + Fragments
void hybrid_partial_match(
    MatchListArray matchesArray,
    FragmentsList* fragments,
    int k,
    SuffixTrie* trie
);

/// Converts an operation enum from int (used internally) to its typed form
Operation get_operation_type(int op);

/// Converts an opArray structure to an Alignment object
Alignment* convert_op_array_to_alignment(opArray ops);

/// Converts a MatchListArray to a CandidatesWithErrors structure
CandidatesWithErrors* convert_matches_to_candidates_with_errors(
    MatchListArray matchesArray,
    FragmentsList* fragments
);

/// Displays the list of candidate fragments with associated errors
void print_candidates_with_errors(CandidatesWithErrors* candidates);

/// Verifies that each candidate with errors actually matches the pattern within k edits
void verify_candidates_with_errors(
    CandidatesWithErrors* candidatesErrors,
    char* text,
    char* pattern,
    int k
);

#endif // BD98D5D6_0618_4155_871E_18C933D1A4A5
