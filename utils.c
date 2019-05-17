#include "utils.h"
static tlv_reply_t *received_reply;
static int fd_log;
void sigalarm_handler_user(int signo) {
  if (signo == SIGALRM) {
    logReply(fd_log, getpid(), received_reply);
    logReply(STDOUT_FILENO, getpid(), received_reply);
    exit(1);
  }
}

void sigaction_execute(void (*func)(int)) {
  struct sigaction action;
  action.sa_flags = 0;
  action.sa_handler = func;
  if (sigaction(SIGALRM, &action, NULL) == -1) {
    perror("SIGALRM");
    exit(1);
  }
}

void change_alarm_signal(void (*func)(int), tlv_reply_t *reply) {
  received_reply = reply;
  sigaction_execute(func);
  alarm(FIFO_TIMEOUT_SECS);
}

void conclude_read() {
  sigaction_execute(SIG_IGN);
}

void fill_reply(tlv_request_t *request, tlv_reply_t *reply) {
  reply->type = request->type;
  reply->value.header.account_id = request->value.header.account_id;
  if (reply->type == OP_BALANCE) {
    reply->length = sizeof(rep_header_t) + sizeof(rep_balance_t);
  }
  else if (reply->type == OP_TRANSFER) {
    reply->length = sizeof(rep_header_t) + sizeof(rep_transfer_t);
  }
  else if (reply->type == OP_SHUTDOWN) {
    reply->length = sizeof(rep_header_t) + sizeof(rep_shutdown_t);
  }
  else {
    reply->length = sizeof(rep_header_t);
  }
}

void close_server_files() {
  unlink(SERVER_FIFO_PATH);
  unlink(SERVER_SEMAPHORE);
}