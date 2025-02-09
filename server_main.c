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

  open_server_log();
  server_cli(argv);
  int fd1;
  atexit(close_server_files);
  if (mkfifo(SERVER_FIFO_PATH, 0660) < 0) {
    fd1 = open(SERVER_FIFO_PATH, O_RDONLY);
    fchmod(fd1, S_IRWXU | S_IRWXG | S_IRWXO);
  }
  while (1) {
    fd1 = open(SERVER_FIFO_PATH, O_RDONLY);
    if (fd1 < 0) {
      perror("Server FIFO");
      exit(1);
    }
    tlv_request_t *request = malloc(sizeof(tlv_request_t));
    memset(request, 0, sizeof(tlv_request_t));
    if (read_request(request, fd1) == -1) {
      free(request);
      close(fd1);
      continue;
    }
    logRequest(get_server_fd(), MAIN_THREAD_ID, request);
    logRequest(STDOUT_FILENO, MAIN_THREAD_ID, request);
    if (request->type == OP_SHUTDOWN) {
      if (initialize_shutdown(request, MAIN_THREAD_ID) == 0) {
        fchmod(fd1, S_IRUSR | S_IRGRP | S_IROTH);
        free(request);
        break;
      }
    }
    produce_data(request);
    close(fd1);
  }

  pthread_exit(0);
}