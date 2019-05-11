#pragma once
#include <fcntl.h>
#include "electronic_branches.h"
#include "pthread.h"
#include "semaphore.h"
#include "types.h"

#define MAX_DATA 200

void initialize_sync(int max_threads);

tlv_request_t *retrieve_data(int thread_id);

void push_data(tlv_request_t *data, int thread_id);

int stop_sync(tlv_request_t * request, int thread_id);