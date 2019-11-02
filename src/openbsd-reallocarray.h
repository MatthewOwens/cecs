#pragma once
#include <sys/types.h>
#include <errno.h>
#include <stdint.h>
#include <stdlib.h>

void * reallocarray(void *optr, size_t nmemb, size_t size);
