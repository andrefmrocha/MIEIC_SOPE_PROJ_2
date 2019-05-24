#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include "constants.h"
#include "sync.h"
#include "types.h"

/**
 * @brief Parsing of the information to send to the server
 * 
 * @param request where to save said information
 * @param argv the command arguments
 */
void user_cli(tlv_request_t *request, char *argv[]);

/**
 * @brief Parsing of the information on how to start the server
 * 
 * @param argv the command arguments
 */
void server_cli(char *argv[]);