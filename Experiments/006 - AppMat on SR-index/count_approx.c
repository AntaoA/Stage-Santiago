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

intervalle_list* count_approx(srIndex* index, char* pattern, int k) {
    infos_queue* queue = create_infos_queue(8);
    int p = strlen(pattern);

    infos* current_info = (infos*)malloc(sizeof(infos));
    current_info->i = p - 1;
    current_info->sp = 1;
    current_info->ep = index->n;
    current_info->err = 0;
    current_info->iv = -1;
    current_info->pv = -1;
    push_infos_queue(queue, current_info);

    intervalle_list* result = (intervalle_list*)malloc(sizeof(intervalle_list));
    result->intervals = (intervalle*)malloc(8 * sizeof(intervalle));
    result->size = 0;
    result->capacity = 8;

    printf("[Start] Pattern = \"%s\", k = %d\n", pattern, k);

    while (queue->front <= queue->rear) {
        infos* info = pop_infos_queue(queue);
        int i = info->i;
        int sp = info->sp;
        int ep = info->ep;
        int err = info->err;
        int iv = info->iv;
        int pv = info->pv;

        printf("\n[Popped] i = %d, sp = %d, ep = %d, err = %d, iv = %d, pv = %d\n", i, sp, ep, err, iv, pv);

        if (err > k) {
            printf("  [Skip] Error limit exceeded: err = %d > k = %d\n", err, k);
            continue;
        }

        if (i < 0) {
            printf("  [Match] Final interval [%d, %d] with err = %d, iv = %d, pv = %d\n", sp, ep, err, iv, pv);
            int v = findToehold_approx(index, pattern, iv, pv);
            printf("  [Toehold] Found toehold position = %d\n", v);

            if (result->size >= result->capacity) {
                result->capacity *= 2;
                result->intervals = (intervalle*)realloc(result->intervals, result->capacity * sizeof(intervalle));
            }
            result->intervals[result->size].sp = sp;
            result->intervals[result->size].ep = ep;
            result->size++;
            continue;
        }

        char c_query = pattern[i];
        printf("  [Char] pattern[%d] = '%c'\n", i, c_query);

        int p_run = rank1(index->start, ep - 1);
        if (c_query != index->letters[p_run - 1]) {
            printf("  [iv/pv update] c_query '%c' ≠ BWT[ep-1] = '%c' → iv = %d, pv = %d\n",
                   c_query, index->letters[p_run - 1], i, p_run);
            iv = i;
            pv = p_run;
        }

        for (int a = 0; a < index->n_letter; a++) {
            char c = index->alphabet[a];
            if (c == '$') continue;

            int run_sp = rank1(index->start, sp - 2);
            int run_ep = rank1(index->start, ep);

            int rank_c_sp = 0, rank_c_ep = 0;
            for (int r = 0; r < run_sp; r++) {
                if (index->letters[r] == c) {
                    int start_pos = select1(index->start, index->n, r);
                    int end_pos = select1(index->start, index->n, r + 1);
                    if (end_pos == -1) end_pos = index->n;
                    if (end_pos > sp - 1) end_pos = sp - 1;
                    rank_c_sp += end_pos - start_pos;
                }
            }
            for (int r = 0; r < run_ep; r++) {
                if (index->letters[r] == c) {
                    int start_pos = select1(index->start, index->n, r);
                    int end_pos = select1(index->start, index->n, r + 1);
                    if (end_pos == -1) end_pos = index->n;
                    if (end_pos > ep) end_pos = ep;
                    rank_c_ep += end_pos - start_pos;
                }
            }

            int sp_new = index->C[letter_conv(c) - 1] + rank_c_sp + 1;
            int ep_new = index->C[letter_conv(c) - 1] + rank_c_ep;

            printf("    [Try] '%c' → sp_new = %d, ep_new = %d\n", c, sp_new, ep_new);

            if (sp_new <= ep_new) {
                int delta = (c_query == c) ? 0 : 1;

                if (err == k && delta > 0) {
                    printf("    [Pruned] Cannot add more errors: skipping substitution '%c'\n", c);
                    continue;
                }

                // Match/Substitution
                infos* new_info = (infos*)malloc(sizeof(infos));
                new_info->i = i - 1;
                new_info->sp = sp_new;
                new_info->ep = ep_new;
                new_info->err = err + delta;
                new_info->iv = iv;
                new_info->pv = pv;
                printf("    [Push] Match/Subst → i=%d, err=%d, iv=%d, pv=%d\n", new_info->i, new_info->err, iv, pv);
                push_infos_queue(queue, new_info);

                // Insertion
                if (err + 1 <= k) {
                    infos* new_info_ins = (infos*)malloc(sizeof(infos));
                    new_info_ins->i = i;
                    new_info_ins->sp = sp_new;
                    new_info_ins->ep = ep_new;
                    new_info_ins->err = err + 1;
                    new_info_ins->iv = iv;
                    new_info_ins->pv = pv;
                    printf("    [Push] Insertion → i=%d, err=%d, iv=%d, pv=%d\n", i, new_info_ins->err, iv, pv);
                    push_infos_queue(queue, new_info_ins);
                }
            } else {
                printf("    [Skip] '%c' → sp_new > ep_new, skip branch\n", c);
            }
        }

        // Deletion
        if (err + 1 <= k) {
            infos* new_info_del = (infos*)malloc(sizeof(infos));
            new_info_del->i = i - 1;
            new_info_del->sp = sp;
            new_info_del->ep = ep;
            new_info_del->err = err + 1;
            new_info_del->iv = iv;
            new_info_del->pv = pv;
            printf("  [Push] Deletion → i=%d, err=%d, iv=%d, pv=%d\n", new_info_del->i, new_info_del->err, iv, pv);
            push_infos_queue(queue, new_info_del);
        }
    }

    free_infos_queue(queue);
    return result;
}