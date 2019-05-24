#include "electronic_branches.h"
extern sem_t empty, full;

static bank_account_t *accounts[MAX_BANK_ACCOUNTS] = {NULL};
static pthread_mutex_t *account_mutexes[MAX_BANK_ACCOUNTS] = {NULL};

void process_data(tlv_request_t *value, int thread_id);

void create_account(tlv_request_t *value, int thread_id);

void check_balance(tlv_request_t *value, int thread_id);

void transfer(tlv_request_t *value, int thread_id);

void *consumer(void *args) {
  int id = *(int *) args;
  tlv_request_t *value;
  logBankOfficeOpen(get_server_fd(), id, pthread_self());
  int sem_value;
  if (args != NULL) {
    while (1) {
      sem_getvalue(&full, &sem_value);
      logSyncMechSem(get_server_fd(), id, SYNC_OP_SEM_WAIT, SYNC_ROLE_CONSUMER, 0, sem_value);
      sem_wait(&full);
      value = retrieve_data(id);
      if (value == NULL)
        break;
      sem_post(&empty);
      sem_getvalue(&empty, &sem_value);
      logSyncMechSem(get_server_fd(), id, SYNC_OP_SEM_POST, SYNC_ROLE_CONSUMER, value->value.header.pid, sem_value);
      logRequest(get_server_fd(), MAIN_THREAD_ID, value);
      process_data(value, id);
    }
  }
  sem_post(&empty);
  sem_getvalue(&empty, &sem_value);
  logSyncMechSem(get_server_fd(), id, SYNC_OP_SEM_POST, SYNC_ROLE_CONSUMER, 0, sem_value);
  logBankOfficeClose(get_server_fd(), id, pthread_self());
  pthread_exit(0);
}

void process_data(tlv_request_t *value, int thread_id) {
  ret_code_t code = login_user(&value->value.header, thread_id);
  if (code == 0) {
    switch (value->type) {
      case OP_CREATE_ACCOUNT:
        create_account(value, thread_id);
        break;

      case OP_BALANCE:
        check_balance(value, thread_id);
        break;

      case OP_TRANSFER:
        transfer(value, thread_id);
        break;

      case OP_SHUTDOWN:
        break;

      default:
        printf("Unrecognized operation!\n");
        sleep(20);
        break;
    }
  }
  else {
    tlv_reply_t reply;
    fill_reply(value, &reply);
    reply.value.header.ret_code = code;
    answer_user(value->value.header.pid, &reply, thread_id);
  }
  decrease_counter(thread_id);
  free(value);
}

void create_account(tlv_request_t *value, int thread_id) {
  ret_code_t code = RC_OTHER;
  if (value->value.header.account_id != ADMIN_ACCOUNT_ID) {
    code = RC_OP_NALLOW;
  }
  else {
    if (account_mutexes[value->value.create.account_id] != NULL) {
      code = RC_ID_IN_USE;
    }
    else {
      save_account(&value->value.create, thread_id);
      code = RC_OK;
    }
  }
  tlv_reply_t reply;
  reply.type = value->type;
  reply.value.header.account_id = value->value.header.account_id;
  reply.value.header.ret_code = code;
  reply.length = sizeof(rep_header_t);
  answer_user(value->value.header.pid, &reply, thread_id);
}

void answer_user(pid_t user_pid, tlv_reply_t *reply, int thread_id) {
  char answer_fifo[USER_FIFO_PATH_LEN];
  char pid[WIDTH_ID + 1];
  sprintf(pid, "%u", user_pid);
  strcpy(answer_fifo, USER_FIFO_PATH_PREFIX);
  strcat(answer_fifo, pid);
  logReply(get_server_fd(), thread_id, reply);
  logReply(STDOUT_FILENO, thread_id, reply);
  tlv_reply_t fail_reply = *reply;
  fail_reply.value.header.ret_code = RC_USR_DOWN;
  int fd = open(answer_fifo, O_WRONLY);
  if (fd < 0) {
    printf("USR Down!\n");
    logReply(get_server_fd(), thread_id, &fail_reply);
    logReply(STDOUT_FILENO, thread_id, &fail_reply);
    return;
  }
  if (write(fd, &reply->type, sizeof(reply->type)) == -1) {
    printf("Failed to write message\n");
  }
  if (write(fd, &reply->length, sizeof(reply->length)) == -1) {
    printf("Failed to write message\n");
  }
  if (write(fd, &reply->value, reply->length) == -1) {
    printf("Failed to write message\n");
  }
}

void save_account(req_create_account_t *account_info, int thread_id) {
  account_mutexes[account_info->account_id] = malloc(sizeof(pthread_mutex_t));
  pthread_mutex_init(account_mutexes[account_info->account_id], NULL);
  logSyncMech(get_server_fd(), thread_id, SYNC_OP_MUTEX_LOCK, SYNC_ROLE_ACCOUNT, account_info->account_id);
  logSyncMech(STDOUT_FILENO, thread_id, SYNC_OP_MUTEX_LOCK, SYNC_ROLE_ACCOUNT, account_info->account_id);
  pthread_mutex_lock(account_mutexes[account_info->account_id]);
  char salt[SALT_LEN];
  char hash[HASH_LEN + 1];
  generate_salt(salt);
  generate_hash(salt, account_info->password, hash);
  accounts[account_info->account_id] = malloc(sizeof(bank_account_t));
  memset(accounts[account_info->account_id], 0, sizeof(bank_account_t));
  accounts[account_info->account_id]
    ->account_id = account_info->account_id;
  accounts[account_info->account_id]->balance = account_info->balance;
  strcpy(accounts[account_info->account_id]->hash, hash);
  strcpy(accounts[account_info->account_id]->salt, salt);
  logAccountCreation(get_server_fd(), thread_id, accounts[account_info->account_id]);
  logAccountCreation(STDOUT_FILENO, thread_id, accounts[account_info->account_id]);
  pthread_mutex_unlock(account_mutexes[account_info->account_id]);
  logSyncMech(get_server_fd(), thread_id, SYNC_OP_MUTEX_UNLOCK, SYNC_ROLE_ACCOUNT, account_info->account_id);
  logSyncMech(STDOUT_FILENO, thread_id, SYNC_OP_MUTEX_UNLOCK, SYNC_ROLE_ACCOUNT, account_info->account_id);
}

int login_user(req_header_t *account, int thread_id) {
  logSyncDelay(get_server_fd(), thread_id, account->account_id, account->op_delay_ms);
  logSyncDelay(STDOUT_FILENO, thread_id, account->account_id, account->op_delay_ms);
  usleep(account->op_delay_ms);
  if (account_mutexes[account->account_id] == NULL) {
    return RC_ID_NOT_FOUND;
  }
  logSyncMech(get_server_fd(), thread_id, SYNC_OP_MUTEX_LOCK, SYNC_ROLE_ACCOUNT, account->account_id);
  logSyncMech(STDOUT_FILENO, thread_id, SYNC_OP_MUTEX_LOCK, SYNC_ROLE_ACCOUNT, account->account_id);
  pthread_mutex_lock(account_mutexes[account->account_id]);
  char hash[HASH_LEN];
  generate_hash(accounts[account->account_id]->salt, account->password, hash);
  if (strcmp(hash, accounts[account->account_id]->hash) != 0) {
    pthread_mutex_unlock(account_mutexes[account->account_id]);
    logSyncMech(get_server_fd(), thread_id, SYNC_OP_MUTEX_UNLOCK, SYNC_ROLE_ACCOUNT, account->account_id);
    logSyncMech(STDOUT_FILENO, thread_id, SYNC_OP_MUTEX_UNLOCK, SYNC_ROLE_ACCOUNT, account->account_id);
    return RC_LOGIN_FAIL;
  }
  else {
    pthread_mutex_unlock(account_mutexes[account->account_id]);
    logSyncMech(get_server_fd(), thread_id, SYNC_OP_MUTEX_UNLOCK, SYNC_ROLE_ACCOUNT, account->account_id);
    logSyncMech(STDOUT_FILENO, thread_id, SYNC_OP_MUTEX_UNLOCK, SYNC_ROLE_ACCOUNT, account->account_id);
    return RC_OK;
  }
}

void check_balance(tlv_request_t *value, int thread_id) {
  tlv_reply_t reply;
  reply.type = value->type;
  reply.value.balance.balance = 0;
  reply.length = sizeof(rep_header_t) + sizeof(rep_balance_t);
  if (value->value.header.account_id == ADMIN_ACCOUNT_ID) {
    reply.value.header.ret_code = RC_OP_NALLOW;
    answer_user(value->value.header.pid, &reply, thread_id);
    return;
  }
  logSyncMech(get_server_fd(), thread_id, SYNC_OP_MUTEX_LOCK, SYNC_ROLE_ACCOUNT, value->value.header.account_id);
  logSyncMech(STDOUT_FILENO, thread_id, SYNC_OP_MUTEX_LOCK, SYNC_ROLE_ACCOUNT, value->value.header.account_id);
  pthread_mutex_lock(account_mutexes[value->value.create.account_id]);
  ret_code_t code = RC_OTHER;
  if (accounts[value->value.header.account_id] == NULL) {
    code = RC_ID_NOT_FOUND;
  }
  else {
    code = RC_OK;
    reply.value.header.account_id = value->value.header.account_id;
    reply.value.balance.balance = accounts[value->value.header.account_id]->balance;
  }
  pthread_mutex_unlock(account_mutexes[value->value.create.account_id]);
  logSyncMech(get_server_fd(), thread_id, SYNC_OP_MUTEX_UNLOCK, SYNC_ROLE_ACCOUNT, value->value.header.account_id);
  logSyncMech(STDOUT_FILENO, thread_id, SYNC_OP_MUTEX_UNLOCK, SYNC_ROLE_ACCOUNT, value->value.header.account_id);
  reply.value.header.ret_code = code;
  answer_user(value->value.header.pid, &reply, thread_id);
}

void transfer(tlv_request_t *value, int thread_id) {
  ret_code_t code = RC_OTHER;
  tlv_reply_t reply;
  reply.type = value->type;
  reply.length = sizeof(rep_header_t) + sizeof(rep_transfer_t);
  reply.value.header.account_id = value->value.header.account_id;
  reply.value.transfer.balance = accounts[value->value.header.account_id]->balance;
  if (value->value.header.account_id == ADMIN_ACCOUNT_ID) {
    reply.value.header.ret_code = RC_OP_NALLOW;
    answer_user(value->value.header.pid, &reply, thread_id);
    return;
  }
  if (accounts[value->value.header.account_id] == NULL || accounts[value->value.transfer.account_id] == NULL) {
    reply.value.header.ret_code = RC_ID_NOT_FOUND;
    answer_user(value->value.header.pid, &reply, thread_id);
    return;
  }
  if (value->value.header.account_id == value->value.transfer.account_id) {
    reply.value.header.ret_code = RC_SAME_ID;
    answer_user(value->value.header.pid, &reply, thread_id);
    return;
  }
  logSyncMech(get_server_fd(), thread_id, SYNC_OP_MUTEX_LOCK, SYNC_ROLE_ACCOUNT, value->value.header.account_id);
  logSyncMech(STDOUT_FILENO, thread_id, SYNC_OP_MUTEX_LOCK, SYNC_ROLE_ACCOUNT, value->value.header.account_id);
  logSyncMech(get_server_fd(), thread_id, SYNC_OP_MUTEX_LOCK, SYNC_ROLE_ACCOUNT, value->value.header.account_id);
  logSyncMech(STDOUT_FILENO, thread_id, SYNC_OP_MUTEX_LOCK, SYNC_ROLE_ACCOUNT, value->value.transfer.account_id);
  if (account_mutexes[value->value.transfer.account_id] > account_mutexes[value->value.header.account_id]){
    pthread_mutex_lock(account_mutexes[value->value.transfer.account_id]);
    pthread_mutex_lock(account_mutexes[value->value.header.account_id]);
  }else{
    pthread_mutex_lock(account_mutexes[value->value.header.account_id]);
    pthread_mutex_lock(account_mutexes[value->value.transfer.account_id]);
  }
  
  if (((int) (accounts[value->value.header.account_id]->balance - value->value.transfer.amount)) < 0) {
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
  if (account_mutexes[value->value.transfer.account_id] > account_mutexes[value->value.header.account_id]) {
    pthread_mutex_unlock(account_mutexes[value->value.transfer.account_id]);
    pthread_mutex_unlock(account_mutexes[value->value.header.account_id]);
  }
  else {
    pthread_mutex_unlock(account_mutexes[value->value.header.account_id]);
    pthread_mutex_unlock(account_mutexes[value->value.transfer.account_id]);
  }
  logSyncMech(get_server_fd(), thread_id, SYNC_OP_MUTEX_UNLOCK, SYNC_ROLE_ACCOUNT, value->value.header.account_id);
  logSyncMech(STDOUT_FILENO, thread_id, SYNC_OP_MUTEX_UNLOCK, SYNC_ROLE_ACCOUNT, value->value.header.account_id);
  logSyncMech(get_server_fd(), thread_id, SYNC_OP_MUTEX_UNLOCK, SYNC_ROLE_ACCOUNT, value->value.transfer.account_id);
  logSyncMech(STDOUT_FILENO, thread_id, SYNC_OP_MUTEX_UNLOCK, SYNC_ROLE_ACCOUNT, value->value.transfer.account_id);
  reply.value.header.ret_code = code;
  answer_user(value->value.header.pid, &reply, thread_id);
}

void free_data() {
  for (int i = 0; i < MAX_BANK_ACCOUNTS; i++) {
    if (accounts[i] != NULL) {
      free(accounts[i]);
      free(account_mutexes[i]);
    }
  }
}