#include "sync.h"
#include "queue.h"

static pthread_mutex_t data_mut = PTHREAD_MUTEX_INITIALIZER;
static pthread_mutex_t counter_mut = PTHREAD_MUTEX_INITIALIZER;
static int num_threads;
static int active_count = 0;
static int *offices_id;
static pthread_t* tid;
static sem_t *sem;
sem_t empty,
  full;

void initialize_sync(int max_threads) {
  num_threads = max_threads;
  sem_init(&empty, 0, max_threads);
  sem_init(&full, 0, 0);
  offices_id = malloc(sizeof(int) * num_threads);
  tid = malloc(sizeof(pthread_t) * num_threads);
  for (int i = 0; i < max_threads; i++) {
    offices_id[i] = i + 1;
    pthread_create(&tid[i], NULL, consumer, &offices_id[i]);
  }

  sem = sem_open(SERVER_SEMAPHORE, O_CREAT, 0600, 0);
  int value;
  sem_getvalue(sem, &value);

  if (sem == SEM_FAILED) {
    printf("Failed to open server semaphore!\n");
    exit(1);
  }
}

tlv_request_t *retrieve_data(int thread_id) {
  tlv_request_t *saving_data = NULL;
  logSyncMech(get_server_fd(), thread_id, SYNC_OP_MUTEX_LOCK, SYNC_ROLE_ACCOUNT, 0);
  logSyncMech(STDOUT_FILENO, thread_id, SYNC_OP_MUTEX_LOCK, SYNC_ROLE_ACCOUNT, 0);
  pthread_mutex_lock(&data_mut);
  if (!isEmpty()) {
    saving_data = removeData();
  }

  pthread_mutex_unlock(&data_mut);
  if (saving_data != NULL) {
    logSyncMech(get_server_fd(), thread_id, SYNC_OP_MUTEX_UNLOCK, SYNC_ROLE_ACCOUNT, saving_data->value.header.pid);
    logSyncMech(STDOUT_FILENO, thread_id, SYNC_OP_MUTEX_UNLOCK, SYNC_ROLE_ACCOUNT, saving_data->value.header.pid);
  }
  else {
    logSyncMech(get_server_fd(), thread_id, SYNC_OP_MUTEX_UNLOCK, SYNC_ROLE_ACCOUNT, 0);
    logSyncMech(STDOUT_FILENO, thread_id, SYNC_OP_MUTEX_UNLOCK, SYNC_ROLE_ACCOUNT, 0);
  }
  logSyncMech(get_server_fd(), thread_id, SYNC_OP_MUTEX_LOCK, SYNC_ROLE_ACCOUNT, 0);
  logSyncMech(STDOUT_FILENO, thread_id, SYNC_OP_MUTEX_LOCK, SYNC_ROLE_ACCOUNT, 0);
  pthread_mutex_lock(&counter_mut);
  active_count++;
  pthread_mutex_unlock(&counter_mut);
  logSyncMech(get_server_fd(), thread_id, SYNC_OP_MUTEX_UNLOCK, SYNC_ROLE_ACCOUNT, 0);
  logSyncMech(STDOUT_FILENO, thread_id, SYNC_OP_MUTEX_UNLOCK, SYNC_ROLE_ACCOUNT, 0);

  return saving_data;
}

void push_data(tlv_request_t *pushing_data, int thread_id) {
  logSyncMech(get_server_fd(), thread_id, SYNC_OP_MUTEX_LOCK, SYNC_ROLE_ACCOUNT, pushing_data->value.header.pid);
  pthread_mutex_lock(&data_mut);
  insert(pushing_data);
  pthread_mutex_unlock(&data_mut);
  logSyncMech(get_server_fd(), thread_id, SYNC_OP_MUTEX_UNLOCK, SYNC_ROLE_ACCOUNT, pushing_data->value.header.pid);
}

int stop_sync(tlv_request_t *request, int thread_id) {
  int sem_value;
  for (int i = 0; i < num_threads; i++) {
    sem_getvalue(&full, &sem_value);
    logSyncMechSem(get_server_fd(), thread_id, SYNC_OP_SEM_POST, SYNC_ROLE_PRODUCER, request->value.header.pid, sem_value);
    sem_post(&full);
  }
  for(int i = 0; i < num_threads; i++){
    pthread_join(tid[i], NULL);
  }
  free_data();
  free(offices_id);
  free(tid);
  sem_close(sem);
  return active_count - num_threads;
}

void next_request() {
  int value;
  sem_getvalue(sem, &value);
  if(value == 0)
    sem_post(sem);
}


void decrease_counter(int thread_id){
  logSyncMech(get_server_fd(), thread_id, SYNC_OP_MUTEX_LOCK, SYNC_ROLE_ACCOUNT, 0);
  logSyncMech(STDOUT_FILENO, thread_id, SYNC_OP_MUTEX_LOCK, SYNC_ROLE_ACCOUNT, 0);
  pthread_mutex_lock(&counter_mut);
  active_count--;
  pthread_mutex_unlock(&counter_mut);
  logSyncMech(get_server_fd(), thread_id, SYNC_OP_MUTEX_UNLOCK, SYNC_ROLE_ACCOUNT, 0);
  logSyncMech(STDOUT_FILENO, thread_id, SYNC_OP_MUTEX_UNLOCK, SYNC_ROLE_ACCOUNT, 0);
}