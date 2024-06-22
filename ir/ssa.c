#include "array.h"
#include "basicblock.h"
#include "function.h"

/*

                alloca %3, 4
                load %4, %3
add %5, %4, 1                   add %6, %4, 1
store %3, %5                    store %3, %6

                load %7, %3

*/

struct Array find_allocas (struct Function *function)
{
}
