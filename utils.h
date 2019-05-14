#pragma once
#include <fcntl.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include "log.h"
#include "sope.h"
#include "types.h"

int open_fifo(char *fifo_name, int flags);

void fill_reply(tlv_request_t *request, tlv_reply_t *reply);

void close_server_files();

void change_alarm_signal(void (*func)(int), tlv_reply_t *reply);

void conclude_read();

void sigalarm_handler_server(int signo);

void sigalarm_handler_user(int signo);


