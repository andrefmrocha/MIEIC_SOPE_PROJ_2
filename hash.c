#include "hash.h"

void generate_salt(char *salt) {
  time_t t;
  srand((unsigned) time(&t));
  for (size_t n = 0; n < SALT_LEN - 2; n++) {
    sprintf(salt + n, "%x", rand() % 0xff);
  }
  salt[SALT_LEN - 1] = '\0';
}

void generate_hash(char *salt, char *password, char *hash) {
  char buffer[strlen(salt) + strlen(password) + 1];
  strcpy(buffer, password);
  strcat(buffer, salt);
  int fd1[2], fd2[2];
  pipe(fd1);
  if (fork() == 0) {
    close(fd1[READ]);
    dup2(fd1[WRITE], STDOUT_FILENO);
    execlp("echo", "echo", "-n", buffer, NULL);
    printf("Failed to execute echo!\n");
    exit(1);
  }
  else {
    close(fd1[WRITE]);
    pipe(fd2);
    if (fork() == 0) {
      close(fd2[READ]);
      dup2(fd2[WRITE], STDOUT_FILENO);
      dup2(fd1[READ], STDIN_FILENO);
      execlp("sha256sum", "sha256sum", NULL);
      printf("Failed to execute sha256sum!\n");
      exit(1);
    }
  }
  close(fd2[WRITE]);
  read(fd2[READ], hash, HASH_LEN);
  hash[HASH_LEN] = '\0';
}
