#pragma once
#include <stdio.h>
#include <unistd.h>
#include "electronic_branches.h"
#include "log.h"
#include "semaphore.h"
#include "sync.h"

/**
 * @brief Produce data into the requests queue, to be consumed down the line by one of the electronic branches
 * 
 * @param request the request itself
 */
void produce_data(tlv_request_t *request);


/**
 * @brief Read the information of a request
 * 
 * @param request where to store said request
 * @param fd1 the file descriptor from where to read the request
 * @return int 0 if sucessful
 */
int read_request(tlv_request_t *request, int fd1);