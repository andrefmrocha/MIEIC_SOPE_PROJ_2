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

    if (request->value.create.account_id < 1 || request->value.create.account_id >= MAX_BANK_ACCOUNTS) {
      printf("Invalid Account ID!\n");
      exit(1);
    }

    if (request->value.create.balance < MIN_BALANCE || request->value.create.balance > MAX_BALANCE) {
      printf("Invalid Balance!\n");
      exit(1);
    }

    if (strlen(request->value.create.password) < MIN_PASSWORD_LEN || strlen(request->value.create.password) > MAX_PASSWORD_LEN) {
      printf("Invalid password length!\n");
      exit(1);
    }

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

    if (strlen(parsed_info[TRNF_ACC_ID]) > WIDTH_ACCOUNT) {
      printf("Invalid account size\n");
      exit(1);
    }

    if (strlen(parsed_info[TRNF_ACC_AMNT]) > WIDTH_BALANCE) {
      printf("Invalid balance size\n");
      exit(1);
    }

    request->value.transfer.account_id = atoi(parsed_info[TRNF_ACC_ID]);
    request->value.transfer.amount = atoi(parsed_info[TRNF_ACC_AMNT]);

    if (request->value.transfer.account_id < 1 || request->value.transfer.account_id >= MAX_BANK_ACCOUNTS) {
      printf("Invalid account ID\n");
    }

    if (request->value.transfer.amount < 1 || request->value.transfer.amount >= MAX_BALANCE) {
      printf("Invalid balance amount \n");
      exit(1);
    }

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

void server_cli(char *argv[]) {
  initialize_sync(MIN(atoi(argv[1]), MAX_BANK_OFFICES));
  req_create_account_t admin_account;
  admin_account.account_id = ADMIN_ACCOUNT_ID;
  admin_account.balance = 0;
  if (strlen(argv[2]) < MIN_PASSWORD_LEN || strlen(argv[2]) > MAX_PASSWORD_LEN) {
    printf("Invalid password size\n");
    exit(1);
  }
  strcpy(admin_account.password, argv[2]);
  save_account(&admin_account);
}