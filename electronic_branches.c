#include "electronic_branches.h"
extern sem_t empty, full;

static bank_account_t *accounts[MAX_BANK_ACCOUNTS] = {NULL};
static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

void process_data(tlv_request_t *value);

void create_account(tlv_request_t *value);

void answer_user(ret_code_t code, tlv_request_t *value);

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
  ret_code_t code = RC_OTHER;
  if (value->value.header.account_id != ADMIN_ACCOUNT_ID) {
    code = RC_OP_NALLOW;
  }
  else {

    pthread_mutex_lock(&mutex);
    if (accounts[value->value.create.account_id] != NULL) {
      code = RC_ID_IN_USE;
    }
    else {
      char salt[SALT_LEN];
      char hash[HASH_LEN];
      generate_salt(salt);
      generate_hash(salt, value->value.create.password, hash);
      printf("Hash: %s\n", hash);
      accounts[value->value.create.account_id] = malloc(sizeof(bank_account_t));
      accounts[value->value.create.account_id]->account_id = value->value.create.account_id;
      accounts[value->value.create.account_id]->balance = value->value.create.balance;
      strcpy(accounts[value->value.create.account_id]->hash, hash);
      strcpy(accounts[value->value.create.account_id]->salt, salt);
      code = RC_OK;
      // printf("Creating account with id %d, balance %d, and password %s\n", value->value.create.account_id, value->value.create.balance, value->value.create.password);
    }
    pthread_mutex_unlock(&mutex);
  }
  answer_user(code, value);
}

void answer_user(ret_code_t code, tlv_request_t *value) {
  printf("Return code: %d\n", code);
  char answer_fifo[USER_FIFO_PATH_LEN];
  char pid[WIDTH_ID + 1];
  sprintf(pid, "%u", value->value.header.pid);
  strcpy(answer_fifo, USER_FIFO_PATH_PREFIX);
  strcat(answer_fifo, pid);
  printf("Answer fifo: %s\n", answer_fifo);
  int fd = open(answer_fifo, O_WRONLY);
  tlv_reply_t reply;
  reply.type = value->type;
  reply.value.header.account_id = value->value.header.account_id;
  reply.value.header.ret_code = code;
  reply.length = sizeof(reply.value);
  write(fd, &reply.type, sizeof(reply.type));
  write(fd, &reply.length, sizeof(reply.length));
  write(fd, &reply.value, reply.length);
}
