#pragma once
#include <pthread.h>
#include <stdio.h>
#include "hash.h"
#include "semaphore.h"
#include "sync.h"

void *consumer(void *args);
