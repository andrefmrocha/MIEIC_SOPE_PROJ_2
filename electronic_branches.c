#include "electronic_branches.h"
extern sem_t empty, full;

void *consumer(void *args) {
  //   int id = *(int *) args;
  if (args != NULL) {
    while (1) {
      sem_wait(&full);
      tlv_request_t *value = retrieve_data();
      if (value == NULL)
        break;
    }
  }
  printf("Found only null information, exiting...\n");
  pthread_exit(0);
}