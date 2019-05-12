#include "producer.h"
extern sem_t empty, full;

void produce_data(tlv_request_t *request) {
  int sem_value;
  sem_getvalue(&empty, &sem_value);
  logSyncMechSem(get_server_fd(), MAIN_THREAD_ID, SYNC_OP_SEM_WAIT, SYNC_ROLE_PRODUCER, request->value.header.pid, sem_value);
  logSyncMechSem(STDOUT_FILENO, MAIN_THREAD_ID, SYNC_OP_SEM_WAIT, SYNC_ROLE_PRODUCER, request->value.header.pid, sem_value);
  sem_wait(&empty);
  push_data(request, MAIN_THREAD_ID);
  sem_post(&full);
  sem_getvalue(&empty, &sem_value);
  logSyncMechSem(get_server_fd(), MAIN_THREAD_ID, SYNC_OP_SEM_POST, SYNC_ROLE_PRODUCER, request->value.header.pid, sem_value);
  logSyncMechSem(STDOUT_FILENO, MAIN_THREAD_ID, SYNC_OP_SEM_POST, SYNC_ROLE_PRODUCER, request->value.header.pid, sem_value);
}

int initialize_shutdown(tlv_request_t *request, int thread_id) {
  tlv_reply_t reply;
  reply.type = OP_SHUTDOWN;
  reply.value.header.account_id = ADMIN_ACCOUNT_ID;
  reply.length = sizeof(rep_header_t) + sizeof(rep_shutdown_t);

  if (request->value.header.account_id == ADMIN_ACCOUNT_ID) {
    logDelay(get_server_fd(), MAIN_THREAD_ID, request->value.header.op_delay_ms);
    logDelay(STDOUT_FILENO, MAIN_THREAD_ID, request->value.header.op_delay_ms);
    usleep(request->value.header.op_delay_ms);
    int active_threads = stop_sync(request, MAIN_THREAD_ID);
    reply.value.header.ret_code = RC_OK;
    reply.value.shutdown.active_offices = active_threads;
    answer_user(request->value.header.pid, &reply, thread_id);
    return 0;
  }
  reply.value.header.ret_code = RC_OP_NALLOW;
  answer_user(request->value.header.pid, &reply, thread_id);
  sleep(5);
  return -1;
}

int read_request(tlv_request_t *request, int fd1) {
  if (read(fd1, &request->type, sizeof(request->type)) == 0) {
    return -1;
  }
  if (read(fd1, &request->length, sizeof(request->length)) == 0) {
    return -1;
  }
  if (read(fd1, &request->value, request->length) == 0) {
    return -1;
  }

  return 0;
}