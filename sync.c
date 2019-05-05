#include "sync.h"

static tlv_request_t *data[MAX_DATA] = {NULL};
static pthread_mutex_t data_mut = PTHREAD_MUTEX_INITIALIZER;
static sem_t *server;

  sem_t empty,
  full;

void initialize_sync(int max_threads) {
  sem_init(&empty, 0, max_threads);
  sem_init(&full, 0, 0);
  server = sem_open(SERVER_SEMAPHORE, O_CREAT | O_EXCL, 0660, 0);
  sem_init(server, 1, 0);
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
