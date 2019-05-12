#include "utils.h"

void sigalarm_handler(int signo) {
  if (signo == SIGUSR1) {
    printf("Server timeout!\n");
    exit(1);
  }
}

void change_alarm_signal(void (*func)(int)) {
  struct sigaction action;
  action.sa_flags = 0;
  action.sa_handler = func;
  if (sigaction(SIGALRM, &action, NULL) == -1) {
    perror("SIGINT");
    exit(1);
  }
}

int open_fifo(char *fifo_name, int flags) {
  change_alarm_signal(sigalarm_handler);
  int fd = open(fifo_name, flags, 0660);
  change_alarm_signal(SIG_DFL);
  if (fd < 0) {
    perror("Failed to open fifo");
    return -1;
  }

  return 0;
}
