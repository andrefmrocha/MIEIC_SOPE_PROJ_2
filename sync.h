#pragma once
#include <fcntl.h>
#include "pthread.h"
#include "semaphore.h"
#include "types.h"
#include "electronic_branches.h"

#define MAX_DATA        200

void initialize_sync(int max_threads);

tlv_request_t * retrieve_data();

void push_data(tlv_request_t * data);