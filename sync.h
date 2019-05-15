#pragma once
#include <fcntl.h>
#include "electronic_branches.h"
#include "log.h"
#include "pthread.h"
#include "semaphore.h"
#include "types.h"

void initialize_sync(int max_threads);

tlv_request_t *retrieve_data(int thread_id);

void push_data(tlv_request_t *data, int thread_id);

int stop_sync(tlv_request_t *request, int thread_id);

void next_request();