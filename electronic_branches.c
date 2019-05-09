#include "electronic_branches.h"
extern sem_t empty, full;

static bank_account_t *accounts[MAX_BANK_ACCOUNTS] = {NULL};
static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

void process_data(tlv_request_t *value);

void create_account(tlv_request_t *value);

void answer_user(pid_t pid, tlv_reply_t *reply);

void check_balance(tlv_request_t *value);

void transfer(tlv_request_t *value);

int login_user(req_header_t *account);

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
  if (login_user(&value->value.header) == 0) {
    switch (value->type) {
      case OP_CREATE_ACCOUNT:
        printf("Create account!\n");
        create_account(value);
        sleep(3);
        break;

      case OP_BALANCE:
        printf("Get account's balance\n");
        check_balance(value);
        sleep(3);
        break;

      case OP_TRANSFER:
        printf("Transfer!\n");
        transfer(value);
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
  else {
    tlv_reply_t reply;
    reply.type = value->type;
    reply.value.header.account_id = value->value.header.account_id;
    reply.value.header.ret_code = RC_LOGIN_FAIL;
    reply.length = sizeof(reply.value);
    printf("Return code: %d\n", RC_LOGIN_FAIL);
    answer_user(value->value.header.pid, &reply);
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
      save_account(&value->value.create);
      code = RC_OK;
    }
    pthread_mutex_unlock(&mutex);
  }
  tlv_reply_t reply;
  reply.type = value->type;
  reply.value.header.account_id = value->value.header.account_id;
  reply.value.header.ret_code = code;
  reply.length = sizeof(reply.value);
  printf("Return code: %d\n", code);
  answer_user(value->value.header.pid, &reply);
}

void answer_user(pid_t user_pid, tlv_reply_t *reply) {
  char answer_fifo[USER_FIFO_PATH_LEN];
  char pid[WIDTH_ID + 1];
  sprintf(pid, "%u", user_pid);
  strcpy(answer_fifo, USER_FIFO_PATH_PREFIX);
  strcat(answer_fifo, pid);
  printf("Answer fifo: %s\n", answer_fifo);
  int fd = open(answer_fifo, O_WRONLY);
  write(fd, &reply->type, sizeof(reply->type));
  write(fd, &reply->length, sizeof(reply->length));
  write(fd, &reply->value, reply->length);
}

void save_account(req_create_account_t *account_info) {
  char salt[SALT_LEN];
  char hash[HASH_LEN];
  generate_salt(salt);
  generate_hash(salt, account_info->password, hash);
  printf("Hash: %s\n", hash);
  accounts[account_info->account_id] = malloc(sizeof(bank_account_t));
  accounts[account_info->account_id]->account_id = account_info->account_id;
  accounts[account_info->account_id]->balance = account_info->balance;
  strcpy(accounts[account_info->account_id]->hash, hash);
  strcpy(accounts[account_info->account_id]->salt, salt);
}

int login_user(req_header_t *account) {
  pthread_mutex_lock(&mutex);
  char hash[HASH_LEN];
  generate_hash(accounts[account->account_id]->salt, account->password, hash);
  if (strcmp(hash, accounts[account->account_id]->hash) != 0) {
    pthread_mutex_unlock(&mutex);
    return -1;
  }
  else {
    pthread_mutex_unlock(&mutex);
    return 0;
  }
}

void check_balance(tlv_request_t *value) {
  ret_code_t code = RC_OTHER;
  tlv_reply_t reply;
  reply.type = value->type;
  pthread_mutex_lock(&mutex);
  if (accounts[value->value.header.account_id] == NULL) {
    code = RC_ID_NOT_FOUND;
  }
  else if (value->value.header.account_id == ADMIN_ACCOUNT_ID) {
    code = RC_OP_NALLOW;
  }
  else {
    code = RC_OK;
    reply.value.header.account_id = value->value.header.account_id;
    reply.value.balance.balance = accounts[value->value.header.account_id]->balance;
    reply.length = sizeof(reply.value);
  }
  pthread_mutex_unlock(&mutex);
  reply.value.header.ret_code = code;
  answer_user(value->value.header.pid, &reply);
}

void transfer(tlv_request_t *value) {
  ret_code_t code = RC_OTHER;
  tlv_reply_t reply;
  reply.type = value->type;
  reply.value.header.account_id = value->value.header.account_id;
  pthread_mutex_lock(&mutex);
  reply.value.transfer.balance = accounts[value->value.header.account_id]->balance;
  if (value->value.header.account_id == ADMIN_ACCOUNT_ID) {
    code = RC_OP_NALLOW;
  }
  else if (accounts[value->value.header.account_id] == NULL || accounts[value->value.transfer.account_id] == NULL) {
    code = RC_ID_NOT_FOUND;
  }
  else if (value->value.header.account_id == value->value.transfer.account_id) {
    code = RC_SAME_ID;
  }
  else if (( (int)(accounts[value->value.header.account_id]->balance - value->value.transfer.amount)) < 0) {
    code = RC_NO_FUNDS;
  }
  else if (accounts[value->value.transfer.account_id]->balance + value->value.transfer.amount > MAX_BALANCE) {
    code = RC_TOO_HIGH;
  }
  else {
    accounts[value->value.header.account_id]->balance -= value->value.transfer.amount;
    accounts[value->value.transfer.account_id]->balance += value->value.transfer.amount;
    code = RC_OK;
    reply.value.transfer.balance = accounts[value->value.header.account_id]->balance;
  }
  reply.value.header.ret_code = code;
  reply.length = sizeof(reply.value);

  answer_user(value->value.header.pid, &reply);
}