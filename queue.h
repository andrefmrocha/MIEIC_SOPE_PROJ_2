#pragma once
struct queue;
typedef struct queue queue_t;


// "constructor":  ini_size, initial queue size
queue_t*new_queue(unsigned int ini_size); 

// queue destructor
void delete_queue(queue_t*q);    

// enqueue ’n’ in queue, returns
//    0 in case of success
//    -1 otherwise (queue full,
//                   or no space)
int put_queue(queue_t*q, int n); 

// dequeue first element from queue to*p, returns
//    0 in case of success
//    -1 otherwise (queue empty)
int get_queue(queue_t*q, int*p); 
