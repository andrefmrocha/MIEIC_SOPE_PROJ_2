#include "utils.h"
static tlv_reply_t * received_reply;
static int fd_log;
void sigalarm_handler(int signo) {
  if (signo == SIGALRM) {
    logReply(fd_log, getpid(), received_reply);
    logReply(STDOUT_FILENO, getpid(), received_reply);
    exit(1);
  }
}

void change_alarm_signal(void (*func)(int)) {
  struct sigaction action;
  action.sa_flags = 0;
  action.sa_handler = func;
  if (sigaction(SIGALRM, &action, NULL) == -1) {
    perror("SIGALRM");
    exit(1);
  }
}

int open_fifo(char *fifo_name, int flags, tlv_reply_t * reply, int logfile) {
  fd_log = logfile;
  received_reply = reply;  
  change_alarm_signal(sigalarm_handler);
  alarm(FIFO_TIMEOUT_SECS);
  int fd = open(fifo_name, flags, 0660);
  change_alarm_signal(SIG_DFL);
  if (fd < 0) {
    perror("Failed to open fifo");
    return -1;
  }

  return fd;
}

void fill_reply(tlv_request_t *request, tlv_reply_t *reply){
  reply->type = request->type;
  reply->value.header.account_id = request->value.header.account_id;
  reply->value.header.ret_code = RC_SRV_DOWN;
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