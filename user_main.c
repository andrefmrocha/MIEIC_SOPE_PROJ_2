#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include "cli.h"
#include "constants.h"
#include "log.h"
#include "sope.h"
#include "types.h"
#include "utils.h"

int main(int argc, char *argv[]) {
  if (argc != 6) {
    printf("Usage: user num_acc password delay_ms operation arguments");
  }
  open_user_log();
  tlv_request_t request;
  user_cli(&request, argv);
  sem_t *sem = sem_open(SERVER_SEMAPHORE, 0, 0600);
  if (sem == SEM_FAILED) {
    printf("Failed to open server semaphore!\n");
    exit(1);
  }
  char answer_fifo[USER_FIFO_PATH_LEN];
  char pid[WIDTH_ID + 1];
  sprintf(pid, "%u", getpid());
  strcpy(answer_fifo, USER_FIFO_PATH_PREFIX);
  strcat(answer_fifo, pid);
  mkfifo(answer_fifo, 0660);
  int value;
  sem_getvalue(sem, &value);
  printf("Sem: %d\n", value);
  sem_wait(sem);
  tlv_reply_t reply;
  fill_reply(&request, &reply);
  reply.value.header.ret_code = RC_SRV_TIMEOUT;
  int fd_server = open(SERVER_FIFO_PATH, O_WRONLY);
  if (fd_server == -1) {
    reply.value.header.ret_code = RC_SRV_DOWN;
    logReply(get_user_fd(), getpid(), &reply);
    logReply(STDOUT_FILENO, getpid(), &reply);
    exit(1);
  }
  logRequest(get_user_fd(), getpid(), &request);
  logRequest(STDOUT_FILENO, getpid(), &request);
  write(fd_server, &request.type, sizeof(op_type_t));
  write(fd_server, &request.length, sizeof(request.length));
  write(fd_server, &request.value, sizeof(request.value));
  close(fd_server);
  fill_reply(&request, &reply);
  change_alarm_signal(sigalarm_handler_user, &reply);
  int fd_answer = open(answer_fifo, O_RDONLY);
  if (fd_answer == -1) {
    printf("Failed to open answer fifo!\n");
    exit(1);
  }
  read(fd_answer, &reply.type, sizeof(reply.type));
  read(fd_answer, &reply.length, sizeof(reply.length));
  read(fd_answer, &reply.value, reply.length);
  conclude_read();
  logReply(get_user_fd(), getpid(), &reply);
  logReply(STDOUT_FILENO, getpid(), &reply);
  unlink(answer_fifo);
  return 0;
}