#pragma once
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include "constants.h"

/**
 * @brief Open server logfile to use along the process
 * 
 */
void open_server_log();

/**
 * @brief Get the server file descriptor
 * 
 * @return int the actual file descriptor
 */
int get_server_fd();

/**
 * @brief Open user logfile to use along the process
 * 
 */
void open_user_log();

/**
 * @brief Get the user file descriptor
 * 
 * @return int the actual file descriptor
 */
int get_user_fd();