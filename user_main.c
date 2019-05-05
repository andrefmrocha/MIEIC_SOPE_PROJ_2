#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include "constants.h"
#include "types.h"

int main() {

  int fd  = open(SERVER_FIFO_PATH, O_RDWR);  
  tlv_request_t request;
  request.type = OP_CREATE_ACCOUNT;
  request.length = sizeof(request.value);
  write(fd, &request.type, sizeof(op_type_t));
  write(fd, &request.length, sizeof(request.length));
  write(fd, &request.value, sizeof(request.value));
  close(fd);
  return 0;
}