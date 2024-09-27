#include "bitmap.h"
#include "mem.h"
#include <stdint.h>
#include "set.h"

void SetInit(struct Set *set, size_t size) {

        BitMapInit(&set->hash_set, size);

}

struct Set * createSet(size_t size) {

        struct Set * set = ir_malloc(sizeof(struct Set));

        SetInit(set, size);

        return set;
}

bool SetContains(struct Set * set, uint64_t value) {

        return BitMapIsSet(&set->hash_set, value);

}


