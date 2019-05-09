#pragma once
#include <stdio.h>
#include <string.h>
#include "constants.h"
#include "time.h"
#include "stdlib.h"

void generate_hash(char *salt, char *password, char *hash);

void generate_salt(char *salt);