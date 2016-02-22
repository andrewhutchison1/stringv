#include "stringv.h"

#include <assert.h>
#include <string.h>

static unsigned blocks_required_by(
        struct stringv const *stringv,
        unsigned string_length);

static char *addressof_nth_block(struct stringv *stringv, unsigned n);
static char *addressof_nth_string(struct stringv *stringv, unsigned n);

static char const *write_string_at_block(
        struct stringv *stringv,
        unsigned block_index,
        unsigned blocks_required,
        char const *string,
        unsigned string_length);

struct stringv *stringv_init(
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
        return NULL;
    }

    stringv->buf = buf;
    stringv->block_total = buf_size / block_size;
    stringv->block_size = block_size;
    stringv->block_used = stringv->string_count = 0;
    memset(stringv->buf, 0, buf_size);

    return stringv;
}

void stringv_clear(struct stringv *stringv)
{
    if (stringv) {
        memset(stringv->buf, 0, stringv->block_total * stringv->block_size);
        stringv->block_used = 0;
        stringv->string_count = 0;
    }
}

struct stringv *stringv_copy(
        struct stringv *dest,
        struct stringv *source,
        enum stringv_error *error)
{
    unsigned i, ith_string_length;
    char *ith_string = NULL;

    if (!dest || !source) {
        error && (*error = stringv_invalid_argument);
        return NULL;
    }

    /* If the destination stringv's block size is smaller than the source's
     * block size, then there may be strings in the source stringv that
     * cannot be represented in the destination stringv. */
    if (dest->block_size < source->block_size) {
        error && (*error = stringv_block_size_mismatch);
        return NULL;
    }

    /* Moreover, if the number of used blocks in the source stringv is greater
     * than the block count in the destination stringv, then there won't be
     * enough room to store all the blocks. */
    if (dest->block_total < source->block_used) {
        error && (*error = stringv_insufficient_blocks);
        return NULL;
    }

    /* Clear the destination stringv */
    stringv_clear(dest);

    for (i = 0; i < source->string_count; ++i) {
        /* Get the address and length of the ith string in the source
         * stringv */
        ith_string = addressof_nth_string(source, i);
        ith_string_length = (unsigned)strlen(ith_string);

        /* Write the ith string in the source stringv to the appropriate
         * block in the destination stringv */
        write_string_at_block(
                dest,
                dest->block_used,
                blocks_required_by(dest, ith_string_length),
                ith_string,
                ith_string_length);
    }

    return dest;
}

char const *stringv_push_back(
        struct stringv *stringv,
        char const *string,
        unsigned string_length,
        enum stringv_error *error)
{
    unsigned blocks_required;

    if (!stringv || !string || string_length == 0) {
        error && (*error = stringv_invalid_argument);
        return NULL;
    }

    /* Determine how many blocks are required by the string in this
     * current stringv */
    blocks_required = blocks_required_by(stringv, string_length);

    /* Ensure that there are sufficient blocks to store the string */
    if (stringv->block_used + blocks_required > stringv->block_total) {
        error && (*error = stringv_insufficient_blocks);
        return NULL;
    }

    /* Write the string at the appropriate block */
    return write_string_at_block(
            stringv,
            stringv->block_used,
            blocks_required,
            string,
            string_length);
}

static char const *write_string_at_block(
        struct stringv *stringv,
        unsigned block_index,
        unsigned blocks_required,
        char const *string,
        unsigned string_length)
{
    char *block_address = NULL;

    assert(stringv);
    assert(block_index < stringv->block_total);
    assert(blocks_required > 0);
    assert(blocks_required + stringv->block_used <= stringv->block_total);
    assert(string);
    assert(string_length > 0);

    /* Get the block address from its index, write the string to this address,
     * then bump the number of used blocks. Under the assumptions that
     * stringv is a valid stringv, we don't need to zero out the remainder
     * of the string's block */
    block_address = addressof_nth_block(stringv, block_index);
    memcpy(block_address, string, string_length);
    stringv->block_used += blocks_required;

    return block_address;
}

static unsigned blocks_required_by(
        struct stringv const *stringv,
        unsigned string_length)
{
    assert(stringv);
    assert(string_length > 0);
    assert(stringv->block_size > 0);

    /* For the NUL char */
    ++string_length;

    /* Round up the required blocks */
    return (string_length / stringv->block_size) +
        (string_length % stringv->block_size != 0);
}

static char *addressof_nth_block(struct stringv *stringv, unsigned n)
{
    assert(stringv);
    assert(n < stringv->block_total);
    return stringv->buf + n * stringv->block_size;
}

static char *addressof_nth_string(struct stringv *stringv, unsigned n)
{
    unsigned m, last_null;

    assert(stringv);
    assert(n < stringv->string_count);

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
