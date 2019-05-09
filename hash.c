#include "hash.h"

void generate_salt(char *salt) {
  time_t t;
  srand((unsigned) time(&t));
  const char charset[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJK...";
  for (size_t n = 0; n < SALT_LEN - 2; n++) {
    int key = rand() % (int) (sizeof charset - 1);
    salt[n] = charset[key];
  }
  salt[SALT_LEN - 1] = '\0';
  printf("Salt: %s\n", salt);
}

void generate_hash(char *salt, char *password, char *hash) {
  char buffer[strlen(salt) + strlen(password) + 1 + strlen("echo ") + strlen(" | sha256sum")];
  strcpy(buffer, "echo ");
  strcat(buffer, password);
  strcat(buffer, salt);
  strcat(buffer, " | sha256sum");
  FILE *file = popen(buffer, "r");
  fgets(hash, HASH_LEN, file);
}