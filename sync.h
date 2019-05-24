#pragma once
#include <fcntl.h>
#include "electronic_branches.h"
#include "log.h"
#include "pthread.h"
#include "semaphore.h"
#include "types.h"

/**
 * @brief Initializing of all synchronization methods used during the process
 * 
 * @param max_threads the maximum of threads (electronic branches) to be initialized
 */
void initialize_sync(int max_threads);

/**
 * @brief Retrieving a request from the queue
 * 
 * @param thread_id the current thread if
 * @return tlv_request_t* the actual request
 */
tlv_request_t *retrieve_data(int thread_id);

/**
 * @brief Pushing a new request into the queue
 * 
 * @param data the new request to be pushed
 * @param thread_id the current thread if
 */
void push_data(tlv_request_t *data, int thread_id);

/**
 * @brief Stopping all synchronization methods, as well as all electronic branches active
 * 
 * @param request the current request
 * @param thread_id the current thread if
 * @return int the number of active threads
 */
int stop_sync(tlv_request_t *request, int thread_id);


/**
 * @brief Decrease active offices counter
 * 
 */
void decrease_counter(int thread_id);