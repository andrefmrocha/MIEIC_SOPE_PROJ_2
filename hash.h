#pragma once
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "constants.h"
#include "stdlib.h"
#include "time.h"

void generate_hash(char *salt, char *password, char *hash);

void generate_salt(char *salt);