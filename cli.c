#include "cli.h"

void user_cli(tlv_request_t *request, char *argv[]) {
  int operation_type = atoi(argv[4]);
  printf("Operation type %d\n", operation_type);
  request->type = operation_type;
  request->value.header.account_id = atoi(argv[1]);
  request->value.header.op_delay_ms = atoi(argv[3]);
  strcpy(request->value.header.password, argv[2]);
  request->length = sizeof(request->value);
}