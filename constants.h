#pragma once

#define MAX_BANK_OFFICES 99
#define MAX_BANK_ACCOUNTS 4096
#define MIN_BALANCE 1UL
#define MAX_BALANCE 1000000000UL
#define ADMIN_ACCOUNT_ID 0
#define MAIN_THREAD_ID 0

#define MIN_PASSWORD_LEN 8
#define MAX_PASSWORD_LEN 20
#define HASH_LEN 64
#define SALT_LEN 64

#define MAX_OP_DELAY_MS 99999

#define WIDTH_ID 5
#define WIDTH_DELAY 5
#define WIDTH_ACCOUNT 4
#define WIDTH_BALANCE 10
#define WIDTH_OP 8
#define WIDTH_RC 12
#define WIDTH_HASH 5
#define WIDTH_TLV_LEN 3
#define WIDTH_STARTEND 5

#define SERVER_LOGFILE "slog.txt"
#define USER_LOGFILE "ulog.txt"

#define SERVER_FIFO_PATH "/tmp/secure_srv"
#define SERVER_SEMAPHORE "/secure_srv_smph"
#define USER_FIFO_PATH_PREFIX "/tmp/secure_"
#define USER_FIFO_PATH_LEN (sizeof(USER_FIFO_PATH_PREFIX) + WIDTH_ID + 1)

#define FIFO_TIMEOUT_SECS 30

#define SPACE_STR " "
#define MAX_ARGUMENTS 20

#define MIN(a, b) (((a) < (b)) ? (a) : (b))

// User CLI Constants
#define ACCOUNT_ID 1
#define OP_DELAY 3
#define PASSWORD 2
#define ARGS 5
#define CRT_ACCOUNT_ID 0
#define CRT_ACCOUNT_BL 1
#define CRT_ACCOUNT_PW 2
#define TRNF_ACC_ID 0
#define TRNF_ACC_AMNT 1

// Server CLI Constants
#define NUM_THRS 3

#define READ    0
#define WRITE   1