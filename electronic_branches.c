#include "electronic_branches.h"
extern sem_t empty, full;

void *consumer(void *args) {
  //   int id = *(int *) args;
  if (args == NULL) {
    while (1) {
      int sem;
      sem_getvalue(&full, &sem);
      printf("Consumer reached semaphore with value %d\n", sem);
      sem_wait(&full);
      tlv_request_t *value = retrieve_data();
      printf("Received request!\n");
      if (value == NULL)
        break;
      sem_post(&empty);
    }
  }
  sem_post(&empty);
  printf("Found only null information, exiting...\n");
  pthread_exit(0);
}