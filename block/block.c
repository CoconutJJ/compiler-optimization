#include "block.h"
#include <stdio.h>
#include <stdlib.h>

Block *create_block ()
{
        Block *blk = malloc (sizeof (Block));

        if (!blk) {
                perror ("malloc");
                exit (EXIT_FAILURE);
        }

        blk->count = 0;
        blk->definitions = NULL;
        blk->jump = NULL;
        blk->fallthrough = NULL;

        return blk;
}
