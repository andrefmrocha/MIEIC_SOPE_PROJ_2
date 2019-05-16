#pragma once
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "types.h"

tlv_request_t *peek();

bool isEmpty();

bool isFull();

int size();

void insert(tlv_request_t *data);

tlv_request_t *removeData();