#pragma once
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "constants.h"
#include "stdlib.h"
#include "time.h"

/**
 * @brief Generate an hash to a given password
 * 
 * @param salt the salt to concatenate to the password
 * @param password the actual password
 * @param hash where to store the hash
 */
void generate_hash(char *salt, char *password, char *hash);

/**
 * @brief Generate a salt, a random number of hexadecimal characters
 * 
 * @param salt where to store the salt
 */
void generate_salt(char *salt);