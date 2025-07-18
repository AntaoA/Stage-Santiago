#include "count_approx.h"




infos_queue* create_infos_queue(int capacity) {
    infos_queue* queue = (infos_queue*)malloc(sizeof(infos_queue));
    if (!queue) {
        return NULL; // Memory allocation failed
    }
    queue->data = (infos*)malloc(capacity * sizeof(infos));
    if (!queue->data) {
        free(queue);
        return NULL; // Memory allocation failed
    }
    queue->front = 0;
    queue->rear = -1;
    queue->capacity = capacity;
    return queue;
}


void free_infos_queue(infos_queue* queue) {
    if (queue) {
        free(queue->data);
        free(queue);
    }
}


infos* pop_infos_queue(infos_queue* queue) {
    if (queue->front > queue->rear) {
        return NULL; // Queue is empty
    }
    infos* info = &queue->data[queue->front];
    queue->front++;
    return info;
}


void push_infos_queue(infos_queue* queue, infos* info) {
    if (queue->rear < queue->capacity - 1) {
        queue->data[++queue->rear] = *info; // Push the info onto the queue
    } else {
        // resize
        queue->capacity *= 2;
        infos* new_data = (infos*)realloc(queue->data, queue->capacity * sizeof(infos));
        if (new_data) {
            queue->data = new_data;
            queue->data[++queue->rear] = *info; // Push the info onto the queue
        } else {
            fprintf(stderr, "Memory allocation failed while resizing the queue.\n");
            exit(EXIT_FAILURE); // Handle memory allocation failure 
        }
    }
}

intervalle_list* count_approx(srIndex* index, char* pattern, int k, int sp_start, int ep_start) {
    infos_queue* queue = create_infos_queue(8);
    int m = strlen(pattern);

    infos* current_info = (infos*)malloc(sizeof(infos));
    current_info->i = m - 1;
    current_info->sp = sp_start;
    current_info->ep = ep_start;
    current_info->err = 0;
    current_info->iv = -1;
    current_info->pv = -1;
    current_info->letter = pattern[m - 1];
    current_info->nb_letters_seen_in_text = 0;
    push_infos_queue(queue, current_info);

    intervalle_list* result = (intervalle_list*)malloc(sizeof(intervalle_list));
    result->intervals = (intervalle*)malloc(8 * sizeof(intervalle));
    result->toeholds = (int*)malloc(8 * sizeof(int));
    result->nb_seen = (int*)malloc(8 * sizeof(int));
    result->size = 0;
    result->capacity = 8;

    while (queue->front <= queue->rear) {
        infos* info = pop_infos_queue(queue);
        int i = info->i;
        int sp = info->sp;
        int ep = info->ep;
        int err = info->err;
        int iv = info->iv;
        int pv = info->pv;
        char letter = info->letter;
        int nb_letters_seen_in_text = info->nb_letters_seen_in_text;
        if (err > k) {
            printf("Skipping info with too many errors: i = %d, sp = %d, ep = %d, err = %d\n", i, sp, ep, err);
            continue;
        }

        printf("Processing info: i = %d, sp = %d, ep = %d, err = %d, iv = %d, pv = %d, letter = '%c'\n", i, sp, ep, err, iv, pv, letter);

        if (i < 0) {
            printf("Found a match: sp = %d, ep = %d, err = %d, iv = %d, pv = %d, letter = '%c'\n", sp, ep, err, iv, pv, letter);
            int toe = findToehold_approx(index, iv, pv, letter, m);


            if (result->size >= result->capacity) {
                result->capacity *= 2;
                result->intervals = (intervalle*)realloc(result->intervals, result->capacity * sizeof(intervalle));
                result->toeholds = (int*)realloc(result->toeholds, result->capacity * sizeof(int));
            }
            result->intervals[result->size].sp = sp;
            result->intervals[result->size].ep = ep;
            result->toeholds[result->size] = toe;
            result->intervals[result->size].errors = err;
            result->nb_seen[result->size] = nb_letters_seen_in_text;
            result->size++;
            continue;
        }

        char c_query = pattern[i];
        int p_run = rank1(index->start, ep - 1);

        for (int a = 0; a < index->n_alphabet; a++) {
            char c = index->alphabet[a];
            if (c == '$') continue;

            int iv_new = iv;
            int pv_new = pv;
            char letter_new = letter;

            if (c != index->letters[p_run - 1]) {
                iv_new = i;
                pv_new = p_run;
                letter_new = c;
            }

            int run_sp = rank1(index->start, sp - 2);
            int run_ep = rank1(index->start, ep - 1);

            int rank_c_sp = 0,rank_c_ep = 0;
            
            for (int r = 0; r < run_sp; r++) {
                if (index->letters[r] == c) {
                    int start_pos = select1(index->start, index->n, r + 1);
                    int end_pos = select1(index->start, index->n, r + 2);
                    if (end_pos == -1) {
                        end_pos = index->n;
                    }
                    if (end_pos > sp - 1) {
                        end_pos = sp - 1;
                    }
                    rank_c_sp += end_pos - start_pos;
                }
            }

            for (int r = 0; r < run_ep; r++) {
                if (index->letters[r] == c) {
                    int start_pos = select1(index->start, index->n, r + 1);
                    int end_pos = select1(index->start, index->n, r + 2);
                    if (end_pos == -1) {
                        end_pos = index->n;
                    }
                    if (end_pos > ep) {
                        end_pos = ep;
                    }
                    rank_c_ep += end_pos - start_pos;
                }
            }

            int sp_new = index->C[letter_conv(c) - 1] + rank_c_sp + 1;
            int ep_new = index->C[letter_conv(c) - 1] + rank_c_ep;


            printf("Processing letter '%c': sp_new = %d, ep_new = %d, iv_new = %d, pv_new = %d, err = %d\n", c, sp_new, ep_new, iv_new, pv_new, err);

            if (sp_new <= ep_new) {
                int delta = (c_query == c) ? 0 : 1;

                if (err == k && delta > 0) continue;

                // Match/Substitution
                infos* new_info = (infos*)malloc(sizeof(infos));
                new_info->i = i - 1;
                new_info->sp = sp_new;
                new_info->ep = ep_new;
                new_info->err = err + delta;
                new_info->iv = iv_new;
                new_info->pv = pv_new;
                new_info->letter = letter_new;
                new_info->nb_letters_seen_in_text = nb_letters_seen_in_text + 1;
                push_infos_queue(queue, new_info);

                // Insertion
                if (err + 1 <= k) {
                    infos* new_info_ins = (infos*)malloc(sizeof(infos));
                    new_info_ins->i = i;
                    new_info_ins->sp = sp_new;
                    new_info_ins->ep = ep_new;
                    new_info_ins->err = err + 1;
                    new_info_ins->iv = (iv_new == -1) ? -1 : iv_new + 1;
                    new_info_ins->pv = pv;
                    new_info_ins->letter = letter;
                    new_info_ins->nb_letters_seen_in_text = nb_letters_seen_in_text + 1;
                    push_infos_queue(queue, new_info_ins);
                }
            }
        }

        // Deletion
        if (err + 1 <= k) {
            infos* new_info_del = (infos*)malloc(sizeof(infos));
            new_info_del->i = i - 1;
            new_info_del->sp = sp;
            new_info_del->ep = ep;
            new_info_del->err = err + 1;
            new_info_del->iv = (iv == -1) ? -1 : iv - 1;
            new_info_del->pv = pv;
            new_info_del->letter = letter;
            new_info_del->nb_letters_seen_in_text = nb_letters_seen_in_text;
            push_infos_queue(queue, new_info_del);
        }
    }

    free_infos_queue(queue);
    return result;
}