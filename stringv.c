#include "stringv.h"

#include <assert.h>
#include <string.h>

int stringv_init(
        struct stringv *stringv,
        char *buf,
        size_t size,
        size_t blocksize)
{
    if (!stringv || !buf || size <= 1 || blocksize <= 1 || blocksize > size) {
        return 0;
    }

    stringv->buf = buf;
    stringv->size = size;
    stringv->blocksize = blocksize;
    stringv->count = 0;

    /* Fill buffer with zero */
    memset(stringv->buf, 0, size);
    return 1;
}
