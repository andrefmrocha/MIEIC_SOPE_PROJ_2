#include "log.h"
static int server_fd, user_fd;

void open_server_log() {
  server_fd = open(SERVER_LOGFILE, O_WRONLY | O_APPEND | O_CREAT, 0660);

  if (server_fd == -1) {
    perror("Failed to open server log");
    exit(1);
  }
}

int get_server_fd() {
  return server_fd;
}

void open_user_log() {
  user_fd = open(USER_LOGFILE, O_WRONLY | O_APPEND | O_CREAT, 0660);

  if (user_fd == -1) {
    perror("Failed to open user log");
    exit(1);
  }
}

int get_user_fd() {
  return user_fd;
}
