#include "electronic_branches.h"
extern sem_t empty, full;


void process_data(tlv_request_t *value);

void *consumer(void *args) {
  //   int id = *(int *) args;
  tlv_request_t *value;
  if (args == NULL) {
    while (1) {
      int sem;
      sem_getvalue(&full, &sem);
      sem_wait(&full);
      value = retrieve_data();
      printf("Received request!\n");
      if (value == NULL)
        break;
      sem_post(&empty);
      process_data(value);
    }
  }
  sem_post(&empty);
  printf("Found only null information, exiting...\n");
  pthread_exit(0);
}

void process_data(tlv_request_t *value) {
  switch (value->type)
  {
    case OP_CREATE_ACCOUNT:
      /* code */
      printf("Create account!\n");
      sleep(3);
      break;
    
    case OP_BALANCE:
      printf("Get account's balance\n");
      sleep(3);
      break;
    
    case OP_TRANSFER:
      printf("Transfer!\n");
      sleep(3);
      break;
    
    case OP_SHUTDOWN:
      printf("Shutdown!\n");
      sleep(3);
      break;
  
    default:
      printf("Unrecognized operation!\n");
      sleep(3);
      break;
  }
}