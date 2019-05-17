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

/**
 * @brief Main consumer loop where all branches shall wait for new requests
 * 
 * @param args its own thread id
 * @return void* 
 */
void *consumer(void *args);

/**
 * @brief Saving the account information to the accounts array
 * 
 * @param account_info the info of that account
 * @param thread_id current thread id
 */
void save_account(req_create_account_t *account_info, int thread_id);

/**
 * @brief Send a reply back to the user, regarding how its request has proceeded
 * 
 * @param user_pid the pid of the user process
 * @param reply the reply to be sent
 * @param thread_id current thread id
 */
void answer_user(pid_t user_pid, tlv_reply_t *reply, int thread_id);

/**
 * @brief Delete all accounts informations. To be called at the end of the process
 * 
 */
void free_data();

/**
 * @brief Login a user 
 * 
 * @param account the actual to login
 * @param thread_id the current thread if
 * @return int RC_OK if sucessful
 */
int login_user(req_header_t *account, int thread_id);