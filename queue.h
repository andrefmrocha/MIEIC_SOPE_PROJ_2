#pragma once
#include "types.h"
struct queue;
typedef struct queue queue_t;

/**
 * @brief queue constructor.
 */
queue_t* new_queue(unsigned int n_el, int el_size);
/**
 * @brief queue destructor.
 */
void delete_queue(queue_t*q);
/**
 * @brief add el to queue.
 */
// returns
//    0 in case of success
//    -1 otherwise (queue full,or no space)
int put_queue(queue_t*q, void*el);
/**
 * @brief dequeue first element from queue to *el
 */
//  returns
//    0 in case of success
//    -1 otherwise (queue empty)
int get_queue(queue_t*q, void*el);

int is_full_queue(queue_t*q);
int is_empty_queue(queue_t*q);