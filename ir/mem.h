#pragma once

#include <stdlib.h>

void *ir_malloc (size_t size);
void *ir_calloc (size_t nmemb, size_t size);
void *ir_realloc (void *mem, size_t size);