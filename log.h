#pragma once
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include "constants.h"

void open_server_log();

int get_server_fd();

void open_user_log();

int get_user_fd();