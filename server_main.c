#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include "cli.h"
#include "constants.h"
#include "producer.h"
#include "semaphore.h"
#include "types.h"

int main(int argc, char *argv[]) {

  if (argc != 3) {
    printf("Usage: server num_offices password");
    exit(1);
  }

  server_cli(argv);
  int fd1;
  mkfifo(SERVER_FIFO_PATH, 0660);

  while (1) {
    fd1 = open(SERVER_FIFO_PATH, O_RDONLY);
    if (fd1 < 0) {
      perror("Server FIFO");
      exit(1);
    }
    tlv_request_t *request = malloc(sizeof(tlv_request_t));
    if (read(fd1, &request->type, sizeof(request->type)) == 0) {
      printf("EOF, continuining...\n");
      continue;
    }
    if (read(fd1, &request->length, sizeof(request->length)) == 0) {
      printf("EOF, continuining...\n");
      continue;
    }
    if (read(fd1, &request->value, request->length) == 0) {
      printf("EOF, continuining...\n");
      continue;
    }
    if(request->type == OP_SHUTDOWN){
      if(initialize_shutdown(request) == 0){
        fchmod(fd1, S_IRUSR | S_IRGRP | S_IROTH);
        break;
      }
    }
    produce_data(request);
    close(fd1);
  }

  unlink(SERVER_FIFO_PATH);
  pthread_exit(0);
}