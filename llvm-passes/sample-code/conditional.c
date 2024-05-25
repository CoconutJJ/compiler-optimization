#include <stdio.h>
#include <stdlib.h>

int main (int argc, char **argv)
{
        int ret_code;

        if (argc > 0) {
                ret_code = 0;
                fprintf (stderr, "No arguments passed");
        } else {
                ret_code = 1;
                fprintf (stderr, "%d arguments passed\n", ret_code);
        }

        return ret_code;
}
