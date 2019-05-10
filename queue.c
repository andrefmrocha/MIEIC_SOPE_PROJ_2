#include "queue.h"

typedef struct {
    int*buf;          // pointer to array that stores queue elements
    int in,out;       // indices of the array pointed by buf to insert/remove elements
    int size;         // size of the array
    int count;        // number of elements in queue
} queue_t;

queue_t* new_queue(unsigned int in_size) {
    // allocate queue object
    queue_t*q = malloc(sizeof(*q));

    if( q == NULL ){
        return NULL;
    }
    // allocate space to store queue elements
    q->size = in_size ? in_size : 1;
    q->buf = malloc(q->size*sizeof(int));
    if( q->buf == NULL ) {
        free(q);return NULL;
    }
    // initialize state of queue
    q->in = q->out = q->count = 0;
    
    return q;
}

void delete_queue(queue_t*q) {
    free(q->buf);
    free(q);
}

int put_queue(queue_t*q, int n) {
    if( q->count == q->size )
        if( resize_queue(q) ) // private function
        return -1;
    q->buf[q->in++] = n;
    q->count++;
    adjust_queue(q); // private function
    return 0;
}

int get_queue(queue_t*q, int*n) {
    if( q->count != 0 ) {
        *n = q->buf[q->out++];
        q->count--;
        adjust_queue(q);
        return 0;
    } 
    return -1;
}

//======= private: can be invoked only in queue.c =========

static void adjust_queue(queue_t*q) {
    q->in %= q->size;
    q->out %= q->size;
}

static int resize_queue(queue_t*q) {
    int*p = (int*)realloc(q->buf, 2*(q->size)*sizeof(int));
    int i;
    if( p == NULL)
        return -1;

    q->buf = p;

    for( i = 0; i < q->in; i++)
        q->buf[q->size + i] = q->buf[i];
    
    q->in += q->size;q->size*= 2;
    return 0;
}