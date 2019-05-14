#include "queue.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

struct queue {
    void*buf;         // void*instead of int*
    int in, out;
    int size, count;
    int el_size;       // for pointer arithmetic
};

queue_t* new_queue(unsigned int n_el, int el_size) {
    queue_t*q = malloc(sizeof(queue_t));
    if( q == NULL )return q;// The user must provide the size of each queue element
    q->size = n_el ? n_el : 1;
    q->buf = malloc(q->size*el_size);
    if( q->buf == NULL ) {free(q);
        return NULL;
    }
    q->in = q->out = q->count = 0;
    q->el_size = el_size;
    return q;
}
void delete_queue(queue_t*q) {
    free(q->buf);
    free(q);
}

int is_empty_queue(queue_t*q) {
    return q->count == 0;
}

int is_full_queue(queue_t*q) {
    return q->count == q->size;
}

int get_queue(queue_t*q, void*el) {
    if( is_empty_queue(q) )
      return -1;
      
    memcpy(el, q->buf + q->out*q->el_size, q->el_size);
    q->out = (q->out + 1) % q->size;q->count--;
    return 0;
}

int put_queue(queue_t*q, void*el) {
    if( is_full_queue(q) )
      return -1;
    // memcpy(dst, src, n_bytes): memory copy
    // must do pointer arithmetic explicitly
    memcpy(q->buf + q->in*q->el_size, el, q->el_size);
    q->in = (q->in + 1) % q->size;
    q->count++;
    return 0;
}