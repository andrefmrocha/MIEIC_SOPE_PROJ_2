#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include "constants.h"
#include "types.h"


int main() {

  int fd1;
  mkfifo(SERVER_FIFO_PATH, 0660);
  while(1){
    fd1 = open(SERVER_FIFO_PATH, O_RDONLY);
    tlv_request_t * request = malloc(sizeof(tlv_request_t));
    read(fd1, &request->type, sizeof(request->type));
    read(fd1, &request->length, sizeof(request->length));
    if(request->length != 0){
      read(fd1, &request->value, request->length);
      printf("Received message type %d, with the length %u\n", request->type, request->length);
    }
    close(fd1);
  }
  

  printf("Hello World!\n");

  return 0;
}