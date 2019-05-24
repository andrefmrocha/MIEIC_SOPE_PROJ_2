#pragma once
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "types.h"

/**
 * @brief Check the current value at the top of the queue
 * 
 * @return tlv_request_t* the request at the top of the queue
 */
tlv_request_t *peek();

/**
 * @brief Check if the queue is empty or not
 * 
 * @return true if it is empty
 * @return false if it isn't empty
 */
bool isEmpty();

/**
 * @brief Check if the queue is full or not
 * 
 * @return true if it is full
 * @return false if it isn't full
 */
bool isFull();

/**
 * @brief Current size of the queue 
 * 
 * @return int number of the requests on the queue
 */
int size();

/**
 * @brief Insert a new request into the queue
 * 
 * @param data the actual request
 */
void insert(tlv_request_t *data);

/**
 * @brief Remove the first value in the queue
 * 
 * @return tlv_request_t* the request removed
 */
tlv_request_t *removeData();