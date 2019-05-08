#include "hash.h"

void generate_salt(char *salt) {
  const char charset[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJK...";
  for (size_t n = 0; n < SALT_LEN - 2; n++) {
    int key = rand() % (int) (sizeof charset - 1);
    salt[n] = charset[key];
  }
  salt[SALT_LEN - 1] = '\0';
  printf("Salt: %s\n", salt);
}

void generate_hash(char *salt, char *password, char *hash) {
  char buffer[strlen(salt) + strlen(password) + 1];
  strcat(buffer, password);
  strcat(buffer, salt);
  FILE *file = popen("echo ola | sha256sum", "r");
  fgets(hash, HASH_LEN, file);
}