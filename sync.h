#pragma once
#include "semaphore.h"
#include "types.h"
#include "pthread.h"

#define MAX_DATA        200

void initialize_sync(int max_threads);

tlv_request_t * retrieve_data();