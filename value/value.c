
#include <stdio.h>
#include <stdlib.h>

static uint32_t value_no = 0;

uint32_t allocate_value_no ()
{
        return value_no++;
}
