#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include "cli.h"
#include "constants.h"
#include "types.h"

int main(int argc, char *argv[]) {
  if (argc != 6) {
    printf("Usage: user num_acc password delay_ms operation arguments");
  }
  tlv_request_t request;
  user_cli(&request, argv);
  int fd_server = open(SERVER_FIFO_PATH, O_RDWR);
  write(fd_server, &request.type, sizeof(op_type_t));
  write(fd_server, &request.length, sizeof(request.length));
  write(fd_server, &request.value, sizeof(request.value));
  close(fd_server);
  char answer_fifo[USER_FIFO_PATH_LEN];
  char pid[WIDTH_ID + 1];
  sprintf(pid, "%u", getpid());
  strcpy(answer_fifo, USER_FIFO_PATH_PREFIX);
  strcat(answer_fifo, pid);
  printf("Answer fifo: %s\n", answer_fifo);
  mkfifo(answer_fifo, 0660);
  int fd_answer = open(answer_fifo, O_RDONLY);
  tlv_reply_t reply;
  read(fd_answer, &reply.type, sizeof(reply.type));
  read(fd_answer, &reply.length, sizeof(reply.length));
  read(fd_answer, &reply.value, reply.length);
  printf("Received answer code %d, from account id %d\n", reply.value.header.ret_code, reply.value.header.account_id);
  if (reply.type == OP_BALANCE) {
    printf("Current balance: %u\n", reply.value.balance.balance);
  }
  if (reply.type == OP_TRANSFER) {
    printf("Current balance: %u\n", reply.value.transfer.balance);
  }
  return 0;
}