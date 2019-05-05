#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include "constants.h"
#include "types.h"
#include "semaphore.h"

int main() {
  int fd  = open(SERVER_FIFO_PATH, O_RDWR);  
  tlv_request_t request;
  request.type = OP_CREATE_ACCOUNT;
  request.length = sizeof(request.value);
  sem_t *server = sem_open(SERVER_SEMAPHORE, 0);
  if(server == NULL){
    printf("Failed to open server!\n");
    exit(1);
  }
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