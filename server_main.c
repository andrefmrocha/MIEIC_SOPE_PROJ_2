#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include "cli.h"
#include "constants.h"
#include "log.h"
#include "producer.h"
#include "semaphore.h"
#include "sope.h"
#include "types.h"

int main(int argc, char *argv[]) {

  if (argc != 3) {
    printf("Usage: server num_offices password \n");
    exit(1);
  }

  server_cli(argv);
  open_server_log();
  int fd1;

  if (mkfifo(SERVER_FIFO_PATH, 0660) == -1){
    perror("Fifo timeout!");
    exit(1);
  }

    while (1) {
      fd1 = open(SERVER_FIFO_PATH, O_RDONLY);
      if (fd1 < 0) {
        perror("Server FIFO");
        exit(1);
      }
      tlv_request_t *request = malloc(sizeof(tlv_request_t));

      if (read_request(request, fd1)) {
        printf("EOF, continuining...\n");
        free(request);
        continue;
      }

      logRequest(get_server_fd(), MAIN_THREAD_ID, request);
      logRequest(STDOUT_FILENO, MAIN_THREAD_ID, request);
      if (request->type == OP_SHUTDOWN) {
        if (initialize_shutdown(request, MAIN_THREAD_ID) == 0) {
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