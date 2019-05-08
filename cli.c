#include "cli.h"

int parse_string(char *string, char *parsed_string[]);

void user_cli(tlv_request_t *request, char *argv[]) {
  enum op_type operation_type = atoi(argv[4]);
  request->type = operation_type;
  request->value.header.account_id = atoi(argv[ACCOUNT_ID]);
  request->value.header.op_delay_ms = atoi(argv[OP_DELAY]);
  strcpy(request->value.header.password, argv[PASSWORD]);
  if (operation_type == OP_CREATE_ACCOUNT) {
    char *parsed_info[MAX_ARGUMENTS];
    if (parse_string(argv[ARGS], parsed_info) != 4) {
      printf("Wrong number of arguments to create an account!\n");
      exit(1);
    }

    if (strlen(parsed_info[CRT_ACCOUNT_ID]) > WIDTH_ACCOUNT) {
      printf("Account argument too big, exiting...\n");
      exit(1);
    }

    if (strlen(parsed_info[CRT_ACCOUNT_BL]) > WIDTH_BALANCE) {
      printf("Balance argument too big, exiting...\n");
      exit(1);
    }
    request->value.create.account_id = atoi(parsed_info[CRT_ACCOUNT_ID]);
    request->value.create.balance = atoi(parsed_info[CRT_ACCOUNT_BL]);
    strcpy(request->value.create.password, parsed_info[CRT_ACCOUNT_PW]);
    printf("Creating account with id %d, balance %d, and password %s\n",
           request->value.create.account_id,
           request->value.create.balance,
           request->value.create.password);
  }
  else if (operation_type == OP_TRANSFER) {
    char *parsed_info[MAX_ARGUMENTS];
    if (parse_string(argv[5], parsed_info) != 3) {
      printf("Wrong number of arguments to make a transfer!\n");
      exit(1);
    }

    request->value.transfer.account_id = atoi(parsed_info[0]);
    request->value.transfer.amount = atoi(parsed_info[1]);

    printf("Transfering %d to account %d\n", request->value.transfer.amount, request->value.transfer.account_id);
  }

  request->length = sizeof(request->value);
  request->value.header.pid = getpid();
}

int parse_string(char *string, char *parsed_string[]) {
  char *token;
  token = strtok(string, SPACE_STR);
  parsed_string[0] = token;
  int i = 1;
  for (; token != NULL; i++) {
    token = strtok(NULL, SPACE_STR);
    parsed_string[i] = token;
  }
  return i;
}