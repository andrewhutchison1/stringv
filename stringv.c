#include "stringv.h"

#include <assert.h>
#include <string.h>

static int blocks_required_by(
        struct stringv const *stringv,
        int string_length);

static char *addressof_nth_block(struct stringv const *stringv, int n);
static char *addressof_nth_string(struct stringv const *stringv, int n);

static int copy_bijective(
        struct stringv *dest,
        struct stringv const *source);

static int copy_injective(
        struct stringv *dest,
        struct stringv const *source);

static int copy_iterative(
        struct stringv *dest,
        struct stringv const *source);

static char const *write_string_at_block(
        struct stringv *stringv,
        int block_index,
        int blocks_required,
        char const *string,
        int string_length);

struct stringv *stringv_init(
        struct stringv *stringv,
        char *buf,
        int buf_size,
        int block_size)
{
    if (!stringv
            || !buf
            || buf_size <= 1
            || block_size <= 1
            || block_size > buf_size) {
        return NULL;
    }

    stringv->buf = buf;
    stringv->block_total = buf_size / block_size;
    stringv->block_size = block_size;
    stringv->block_used = stringv->string_count = 0;
    memset(stringv->buf, 0, buf_size);

    return stringv;
}

struct stringv *stringv_clear(struct stringv *stringv)
{
    if (stringv) {
        memset(stringv->buf, 0, stringv->block_total * stringv->block_size);
        stringv->block_used = 0;
        stringv->string_count = 0;
    }

    return stringv;
}

int stringv_copy(
        struct stringv *dest,
        struct stringv const *source)
{
    if (!dest || !source) {
        return 0;
    }

    /* Clear the destination stringv */
    (void)stringv_clear(dest);

    /* If the source stringv is empty, don't do anything. This is done after
     * stringv_clear because clearing the destination stringv may be a side
     * effect that is relied upon */
    if (source->string_count == 0) {
        return 0;
    }

    /* If the destination stringv has the same block size, then we can just
     * memcpy the entire block over. It also needs to have the same or a
     * greater block total. */
    if (dest->block_size == source->block_size
            && dest->block_total >= source->block_total) {
        return copy_bijective(dest, source);
    }

    /* If the source stringv has the same number of used blocks as its string
     * count, and the destination's block size is greater than or equal to that
     * of the source, then we know for certain that each string only
     * requires a single block in the destination stringv. This means we can
     * skip computing the required blocks for each string */
    if (dest->block_size >= source->block_size
            && source->block_used == source->string_count) {
        return copy_injective(dest, source);
    }

    /* We can't optimise the copy operation, so just do it iteratively. */
    return copy_iterative(dest, source);
}

char const *stringv_push_back(
        struct stringv *stringv,
        char const *string,
        int length)
{
    int blocks_required = 0;

    if (!stringv || !string || length <= 0) {
        return NULL;
    }

    /* Determine how many blocks are required by the string in this
     * current stringv */
    blocks_required = blocks_required_by(stringv, length);

    /* Ensure that there are sufficient blocks to store the string */
    if (stringv->block_used + blocks_required > stringv->block_total) {
        return NULL;
    }

    /* Write the string at the appropriate block */
    return write_string_at_block(
            stringv,
            stringv->block_used,
            blocks_required,
            string,
            length);
}

int blocks_required_by(
        struct stringv const *stringv,
        int string_length)
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

char *addressof_nth_block(struct stringv const *stringv, int n)
{
    assert(stringv);
    assert(n >= 0);
    assert(n < stringv->block_total);
    return stringv->buf + n * stringv->block_size;
}

char *addressof_nth_string(struct stringv const *stringv, int n)
{
    int i = 0, last_nul = 0;

    assert(stringv);
    assert(n >= 0);
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
     * this location is NUL, the next char is the start of the nth block */
    last_nul = stringv->block_total * stringv->block_size - 1;
    for (i = stringv->block_size - 1;
            i != last_nul;
            i += stringv->block_size)
    {
        /* Landing on a NUL character means that the next character starts
         * a block. So we decrement the block index to signify a block
         * being encountered */
        if (!stringv->buf[i]) {
            --n;
        }

        /* When n is zero, we are are the desired block index. The next
         * character would be the start character of the desired block */
        if (n == 0) {
            break;
        }
    }

    assert(n == 0);
    return stringv->buf + i + 1;
}

int copy_bijective(
        struct stringv *dest,
        struct stringv const *source)
{
    assert(dest);
    assert(source);
    assert(source->block_size == dest->block_size);
    assert(source->block_used <= dest->block_total);

    memcpy(dest->buf, source->buf, source->block_size * source->block_used);
    dest->string_count = source->string_count;
    dest->block_used = source->block_used;
    return source->string_count;
}

int copy_injective(
        struct stringv *dest,
        struct stringv const *source)
{
    int i = 0;

    assert(dest);
    assert(source);
    assert(source->block_size <= dest->block_size);
    assert(source->block_used == source->string_count);

    /* We can simply loop through the source stringv's strings, writing them
     * to the destination stringv as we go. Note that the blocks required are
     * not computed at each iteration, instead they are hardcoded as 1. For
     * the string length argument, we just give it the source block size
     * subtracted by 1. This is because the destination block size is larger
     * than or equal to the source block size, and we don't need to recompute
     * the string length */
    for (; i < source->string_count; ++i) {
        write_string_at_block(
                dest,
                dest->block_used,
                1,
                addressof_nth_string(source, i),
                source->block_size - 1);
    }

    return source->string_count;
}

int copy_iterative(
        struct stringv *dest,
        struct stringv const *source)
{
    int i = 0, ith_string_length = 0, strings_copied = 0;
    char *ith_string = NULL;

    assert(dest);
    assert(source);

    for (; i < source->string_count; ++i) {
        ith_string = addressof_nth_string(source, i);
        ith_string_length = (int)strlen(ith_string);

        if (write_string_at_block(
                dest,
                dest->block_used,
                blocks_required_by(dest, ith_string_length),
                ith_string,
                ith_string_length)) {
            ++strings_copied;
        } else {
            break;
        }
    }

    return strings_copied;
}

char const *write_string_at_block(
        struct stringv *stringv,
        int block_index,
        int blocks_required,
        char const *string,
        int string_length)
{
    char *block_address = NULL;

    assert(stringv);
    assert(block_index >= 0);
    assert(block_index < stringv->block_total);
    assert(blocks_required > 0);
    assert(string);
    assert(string_length > 0);

    if (blocks_required + stringv->block_used > stringv->block_total) {
        return NULL;
    }

    /* Get the block address from its index and then copy over the string data
     * to it. We only copy the string data (and not its NUL terminator) since
     * the remainder of the buffer should be zero under the assumption that
     * stringv is a valid stringv */
    block_address = addressof_nth_block(stringv, block_index);
    memcpy(block_address, string, string_length);

    /* Bump number of blocks and string count */
    stringv->block_used += blocks_required;
    ++stringv->string_count;

    return block_address;
}
