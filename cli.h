#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "constants.h"
#include "sync.h"
#include "types.h"

void user_cli(tlv_request_t *request, char *argv[]);

void server_cli(char *argv[]);