#include "electronic_branches.h"
extern sem_t empty, full;

static bank_account_t *accounts[MAX_BANK_ACCOUNTS] = {NULL};
static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

void process_data(tlv_request_t *value);

void create_account(tlv_request_t *value);

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
  switch (value->type) {
    case OP_CREATE_ACCOUNT:
      /* code */
      printf("Create account!\n");
      create_account(value);
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

void create_account(tlv_request_t *value) {
  if (value->value.header.account_id != ADMIN_ACCOUNT_ID) {
    // TODO:  send OP_NOT_ALLOW
  }

  pthread_mutex_lock(&mutex);
  if (accounts[value->value.create.account_id] != NULL) {
    //TODO: send ID_IN_USE
  }

  char salt[SALT_LEN];
  char hash[HASH_LEN];
  generate_salt(salt);
  generate_hash(salt, value->value.create.password, hash);
  printf("Hash: %s\n", hash);
}