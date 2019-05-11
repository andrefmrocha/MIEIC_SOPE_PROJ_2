#pragma once
#include <stdio.h>
#include "semaphore.h"
#include "electronic_branches.h"
#include "sync.h"

void produce_data(tlv_request_t *request);

int initialize_shutdown(tlv_request_t *request, int thread_id);

int read_request(tlv_request_t *request, int fd1);