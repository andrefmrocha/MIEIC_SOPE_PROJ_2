#include "cli.h"

int parse_string(char *string, char *parsed_string[]);

void user_cli(tlv_request_t *request, char *argv[]) {
  enum op_type operation_type = atoi(argv[4]);
  request->type = operation_type;
  request->value.header.account_id = atoi(argv[1]);
  request->value.header.op_delay_ms = atoi(argv[3]);
  strcpy(request->value.header.password, argv[2]);
  if (operation_type == OP_CREATE_ACCOUNT) {
    char *parsed_info[MAX_ARGUMENTS];
    if (parse_string(argv[5], parsed_info) != 4) {
      printf("Wrong number of arguments to create an account!\n");
      exit(1);
    }
    request->value.create.account_id = atoi(parsed_info[0]);
    request->value.create.balance = atoi(parsed_info[1]);
    strcpy(request->value.create.password, parsed_info[2]);
    printf("Creating account with id %d, balance %d, and password %s\n",
           request->value.create.account_id,
           request->value.create.balance,
           request->value.create.password);
  }
  else if (operation_type == OP_TRANSFER) {
    char *parsed_info[MAX_ARGUMENTS];
    if (parse_string(argv[5], parsed_info) != 3){
      printf("Wrong number of arguments to make a transfer!\n");
      exit(1);
    }

    request->value.transfer.account_id = atoi(parsed_info[0]);
    request->value.transfer.amount = atoi(parsed_info[1]);

    printf("Transfering %d to account %d\n", request->value.transfer.amount, request->value.transfer.account_id);

  }

  request->length = sizeof(request->value);
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