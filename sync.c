#include "sync.h"
#include "queue.h"

//static tlv_request_t *data[MAX_DATA] = {NULL};
static pthread_mutex_t data_mut = PTHREAD_MUTEX_INITIALIZER;
static int num_threads;
static int *offices_id;
static sem_t * sem;
sem_t empty,
  full;

void initialize_sync(int max_threads) {
  num_threads = max_threads;
  sem_init(&empty, 0, max_threads);
  sem_init(&full, 0, 0);
  offices_id = malloc(sizeof(int) * num_threads);
  for (int i = 0; i < max_threads; i++) {
    pthread_t tid;
    offices_id[i] = i + 1;
    pthread_create(&tid, NULL, consumer, &i);
  }

  sem = sem_open(SERVER_SEMAPHORE, O_CREAT, 0600, 0);
  int value;
  sem_getvalue(sem, &value);
  printf("Semaphore %d\n", value);

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
  printf("Accesing queue\n");
  saving_data = removeData();
/*
  for (int i = 0; i < MAX_DATA; i++) {
    if (data[i] != NULL) {
      saving_data = data[i];
      data[i] = NULL;
      break;
    }
  }*/
  pthread_mutex_unlock(&data_mut);
  logSyncMech(get_server_fd(), thread_id, SYNC_OP_MUTEX_UNLOCK, SYNC_ROLE_ACCOUNT, saving_data->value.header.pid);
  logSyncMech(STDOUT_FILENO, thread_id, SYNC_OP_MUTEX_UNLOCK, SYNC_ROLE_ACCOUNT, saving_data->value.header.pid);
  return saving_data;
}

void push_data(tlv_request_t *pushing_data, int thread_id) {
  logSyncMech(get_server_fd(), thread_id, SYNC_OP_MUTEX_LOCK, SYNC_ROLE_ACCOUNT, pushing_data->value.header.pid);
  logSyncMech(STDOUT_FILENO, thread_id, SYNC_OP_MUTEX_LOCK, SYNC_ROLE_ACCOUNT, pushing_data->value.header.pid);
  pthread_mutex_lock(&data_mut);

  insert(pushing_data);

    /*
  for (int i = 0; i < MAX_DATA; i++) {
    if (data[i] == NULL) {
      data[i] = pushing_data;
      break;
    }
  }*/
  pthread_mutex_unlock(&data_mut);
  logSyncMech(get_server_fd(), thread_id, SYNC_OP_MUTEX_UNLOCK, SYNC_ROLE_ACCOUNT, pushing_data->value.header.pid);
  logSyncMech(STDOUT_FILENO, thread_id, SYNC_OP_MUTEX_UNLOCK, SYNC_ROLE_ACCOUNT, pushing_data->value.header.pid);
}

int stop_sync(tlv_request_t *request, int thread_id) {
  int full_num, sem_value;
  sem_getvalue(&full, &full_num);
  for (int i = 0; i < num_threads; i++) {
    sem_getvalue(&full, &sem_value);
    logSyncMechSem(get_server_fd(), thread_id, SYNC_OP_SEM_POST, SYNC_ROLE_PRODUCER, request->value.header.pid, sem_value);
    logSyncMechSem(STDOUT_FILENO, thread_id, SYNC_OP_SEM_POST, SYNC_ROLE_PRODUCER, request->value.header.pid, sem_value);
    sem_post(&full);
  }
  return full_num;
}

void next_request(){
  sem_post(sem);
}