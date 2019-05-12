#pragma once
#include <fcntl.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include "types.h"
#include "sope.h"
#include "log.h"

int open_fifo(char *fifo_name, int flags, tlv_reply_t * reply, int fd);

void fill_reply(tlv_request_t * request, tlv_reply_t * reply);