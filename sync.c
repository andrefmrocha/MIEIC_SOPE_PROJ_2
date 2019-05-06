#include "sync.h"

static tlv_request_t *data[MAX_DATA] = {NULL};
static pthread_mutex_t data_mut = PTHREAD_MUTEX_INITIALIZER;

  sem_t empty,
  full;

void initialize_sync(int max_threads) {
  sem_init(&empty, 0, max_threads);
  sem_init(&full, 0, 0);
  for(int i = 0; i < max_threads; i++) {
    pthread_t tid;
    pthread_create(&tid, NULL, consumer, NULL);
  }
}

tlv_request_t *retrieve_data() {
  for (int i = 0; i < MAX_DATA; i++) {
    pthread_mutex_lock(&data_mut);
    if (data[i] != NULL) {
      pthread_mutex_unlock(&data_mut);
      return data[i];
    }
  }
  pthread_mutex_unlock(&data_mut);
  return NULL;
}

void push_data(tlv_request_t *pushing_data){
  for (int i = 0; i < MAX_DATA; i++){
    pthread_mutex_lock(&data_mut);
    if(data[i] == NULL){
      data[i] = pushing_data;
      break;
    }
  }
  pthread_mutex_unlock(&data_mut);
}