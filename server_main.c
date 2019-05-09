#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include "constants.h"
#include "producer.h"
#include "semaphore.h"
#include "sync.h"
#include "types.h"

int main() {

  int fd1;
  mkfifo(SERVER_FIFO_PATH, 0660);
  initialize_sync(3);
  req_create_account_t admin_account;
  admin_account.account_id = ADMIN_ACCOUNT_ID;
  admin_account.balance = 0;
  strcpy(admin_account.password, "wrong_password");
  save_account(&admin_account);

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

    produce_data(request);
    close(fd1);
  }

  unlink(SERVER_FIFO_PATH);
  printf("Hello World!\n");

  return 0;
}