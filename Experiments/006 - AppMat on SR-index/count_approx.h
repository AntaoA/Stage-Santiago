#ifndef D5E759BE_A0C2_4CF1_B236_C568518C77C4
#define D5E759BE_A0C2_4CF1_B236_C568518C77C4

#include "../005 - SR-index/sr-index.h"
#include "../005 - SR-index/utils.h"
#include "findToehold_approx.h"

typedef struct {
    int sp;
    int ep;
    int errors;
} intervalle;

typedef struct {
    intervalle* intervals;
    int* toeholds;
    int* nb_seen;
    int size;
    int capacity;
} intervalle_list;

typedef struct {
    int i;
    int sp;
    int ep;
    int err;
    int iv;
    int pv;
    char letter;
    int nb_letters_seen_in_text;
} infos;

typedef struct infosQueue {
    infos* data;
    int front;
    int rear;
    int capacity;
} infos_queue;

infos_queue* create_infos_queue(int capacity);
void free_infos_queue(infos_queue* queue);
infos* pop_infos_queue(infos_queue* queue);
void push_infos_queue(infos_queue* queue, infos* info);

intervalle_list* count_approx(srIndex* index, char* pattern, int k, int sp_start, int ep_start);


#endif /* D5E759BE_A0C2_4CF1_B236_C568518C77C4 */
