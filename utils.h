#pragma once
#include <fcntl.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include "log.h"
#include "sope.h"
#include "types.h"


/**
 * @brief Fill a reply with the information regarding the current request
 * 
 * @param request the current request
 * @param reply where to store said reply
 */
void fill_reply(tlv_request_t *request, tlv_reply_t *reply);

/**
 * @brief Close all server files when they are not needed anymore
 * 
 */
void close_server_files();

/**
 * @brief Change the signal related the SIGALRM signal
 * 
 * @param func the function to use
 * @param reply the current reply
 */
void change_alarm_signal(void (*func)(int), tlv_reply_t *reply);

/**
 * @brief Deactivate SIGALRM signal after reading is concluded
 * 
 */
void conclude_read();

/**
 * @brief Handler for the SIGALRM signal regarding server timeout
 * 
 * @param signo the signal number
 */
void sigalarm_handler_user(int signo);


void save_answer_fifo(char *answer);
void unlink_answer_fifo();