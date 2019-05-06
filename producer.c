#include "producer.h"
extern sem_t empty, full;

void produce_data(tlv_request_t *request){
    sem_wait(&empty);
    push_data(request);
    sem_post(&full);
}