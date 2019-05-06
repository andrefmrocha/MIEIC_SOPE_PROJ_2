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
  int fd = open(SERVER_FIFO_PATH, O_RDWR);
  // int value;
  // sem_getvalue(server, &value);
  // printf("Value %d\n", value);
  write(fd, &request.type, sizeof(op_type_t));
  // sem_post(server);
  // sem_wait(server);
  write(fd, &request.length, sizeof(request.length));
  // sem_post(server);
  // sem_wait(server);
  write(fd, &request.value, sizeof(request.value));
  // sem_post(server);
  close(fd);
  return 0;
}