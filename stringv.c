#include "stringv.h"

#include <assert.h>
#include <string.h>

int stringv_init(
        struct stringv *stringv,
        char *buf,
        size_t size,
        size_t blocksize)
{
    if (!stringv || !buf || size <= 1 || blocksize <= 1) {
        return 0;
    }

    stringv->buf = buf;
    stringv->size = size;
    stringv->blocksize = blocksize;

    /* Fill buffer with zero */
    memset(stringv->buf, 0, size);

    return 1;
}

int stringv_capacity(struct stringv const *stringv)
{
    if (!stringv) {
        return 0;
    }

    /* The optimum capacity for a given stringv is acheived when the length
     * of all strings is less than the block size. We also have to keep in
     * mind that there may be dead space at the end of the stringv's buffer.
     * This is essentially the algebraic quotient of the buffer size and
     * block size. */
    return stringv->size / stringv->blocksize;
}
