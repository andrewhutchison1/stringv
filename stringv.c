#include "stringv.h"

#include <assert.h>
#include <string.h>

static unsigned blocks_required_by(
        struct stringv const *stringv,
        unsigned string_length);

static char *addressof_nth_block(struct stringv *stringv, unsigned n);
static char *addressof_nth_string(struct stringv *stringv, unsigned n);

int stringv_init(
        struct stringv *stringv,
        char *buf,
        unsigned buf_size,
        unsigned block_size)
{
    if (!stringv ||
            !buf ||
            buf_size <= 1 ||
            block_size <= 1 ||
            block_size > buf_size) {
        return 0;
    }

    stringv->buf = buf;
    stringv->block_total = buf_size / block_size;
    stringv->block_size = block_size;
    stringv->block_used = stringv->string_count = 0;
    memset(stringv->buf, 0, buf_size);

    return 1;
}

int stringv_copy(
        struct stringv *dest,
        struct stringv *source)
{
    unsigned i, block, ith_string_length;
    char *ith_string = NULL;

    if (!dest || !source) {
        return 0;
    }

    /* If the destination stringv's block size is smaller than the source's
     * block size, then there may be strings in the source stringv that
     * cannot be represented in the destination stringv. */
    if (dest->block_size < source->block_size) {
        return 0;
    }

    /* Moreover, if the number of used blocks in the source stringv is greater
     * than the block count in the destination stringv, then there won't be
     * enough room to store all the blocks. */
    if (dest->block_total < source->block_used) {
        return 0;
    }

    for (block = 0, i = 0; i < source->string_count; ++i) {
        /* Get the address of the ith string in the source stringv, and its
         * length. We can use strlen here without fear of buffer overrun if
         * we assume that the stringv invariant (buffer initially zeroed)
         * holds. */
        ith_string = addressof_nth_string(source, i);
        ith_string_length = (unsigned)strlen(ith_string);

        /* Copy the ith string in the source stringv to the address of the
         * block that we are up to */
        memcpy(addressof_nth_block(dest, block),
                ith_string,
                ith_string_length);

        /* Now skip to the next block index in the destination stringv
         * immediately after the string we just wrote */
        block += blocks_required_by(dest, ith_string_length);
    }

    return 1;
}

int stringv_push_back(
        struct stringv *stringv,
        char const *string,
        unsigned string_length)
{
    if (!stringv || !string || string_length == 0) {
        return 0;
    }
}

static unsigned blocks_required_by(
        struct stringv const *stringv,
        unsigned string_length)
{
    assert(stringv && (string_length > 0));

    /* For the NUL char */
    ++string_length;

    /* Round up the required blocks */
    return (string_length / stringv->block_size) +
        (string_length % stringv->block_size != 0);
}

static char *addressof_nth_block(struct stringv *stringv, unsigned n)
{
    assert(stringv && n < stringv->block_total);
    return stringv->buf + n * stringv->block_size;
}

static char *addressof_nth_string(struct stringv *stringv, unsigned n)
{
    unsigned m, last_null;

    assert(stringv && n < stringv->string_count);

    /* The first string always starts at the start of the stringv's buffer */
    if (n == 0) {
        return stringv->buf;
    }

    /* We can perform an optimisation here if the stringv's string count is
     * equal to the number of used blocks (ie. there is a bijection
     * between string addresses and block addresses). Since an arbitrary
     * block address can be determined in constant time, we can defer to
     * addressof_nth_block instead with the same result. */
    if (stringv->string_count == stringv->block_used) {
        return addressof_nth_block(stringv, n);
    }

    /* Otherwise, we need to iterate through the buffer in steps of
     * block_size, starting from block_size - 1. If the char at
     * this location is 0, the next char is the start of the nth block */
    last_null = stringv->block_total * stringv->block_size - 1;
    for (m = stringv->block_size - 1;
            m != last_null;
            m += stringv->block_size)
    {
        /* Landing on a NUL character means that the next character starts
         * a block. So we decrement the block index to signify a block
         * being encountered */
        if (stringv->buf[m] == '\0') {
            --n;
        }

        /* When n is zero, we are are the desired block index. The next
         * character would be the start character of the desired block */
        if (n == 0) {
            break;
        }
    }

    assert(n == 0);
    return stringv->buf + m + 1;
}
