#include "stringv.h"

#include <assert.h>
#include <string.h>

int stringv_init(
        struct stringv *stringv,
        char *buf,
        unsigned size,
        unsigned blocksize)
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

int stringv_copy(
        struct stringv *dest,
        struct stringv *source)
{
    if (!dest || !source) {
        return 0;
    }

    /* If the blocksize of the destination stringv is smaller than the 
     * blocksize of source, then we can't copy the strings over since there
     * may be strings too large to represent in the destination stringv */
    if (dest->blocksize < source->blocksize) {
        return 0;
    }


}
