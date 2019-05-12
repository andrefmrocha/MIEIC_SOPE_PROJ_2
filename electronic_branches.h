#pragma once
#include <pthread.h>
#include <stdio.h>
#include <unistd.h>
#include "hash.h"
#include "log.h"
#include "semaphore.h"
#include "sope.h"
#include "sync.h"
#include "types.h"
#include "utils.h"

void *consumer(void *args);

void save_account(req_create_account_t *account_info, int thread_id);

void answer_user(pid_t user_pid, tlv_reply_t *reply, int thread_id);
