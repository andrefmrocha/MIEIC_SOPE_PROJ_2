#include "producer.h"
extern sem_t empty, full;

void produce_data(tlv_request_t *request) {
  int sem_value;
  sem_getvalue(&empty, &sem_value);
  logSyncMechSem(get_server_fd(), MAIN_THREAD_ID, SYNC_OP_SEM_WAIT, SYNC_ROLE_PRODUCER, request->value.header.pid, sem_value);
  sem_wait(&empty);
  push_data(request, MAIN_THREAD_ID);
  sem_post(&full);
  sem_getvalue(&empty, &sem_value);
  logSyncMechSem(get_server_fd(), MAIN_THREAD_ID, SYNC_OP_SEM_POST, SYNC_ROLE_PRODUCER, request->value.header.pid, sem_value);
}


int read_request(tlv_request_t *request, int fd1) {
  if (read(fd1, &request->type, sizeof(request->type)) <= 0) {
    return -1;
  }
  if (read(fd1, &request->length, sizeof(request->length)) <= 0) {
    return -1;
  }
  if (read(fd1, &request->value, request->length) <= 0) {
    return -1;
  }
  if (request->length > sizeof(tlv_request_t))
    return -1;
  return 0;
}
