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

static char *shift_blocks_forward(
        struct stringv *stringv,
        int block_index,
        int n);

static char *shift_blocks_backward(
        struct stringv *stringv,
        int block_index,
        int n);

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

char const *stringv_get(struct stringv const *stringv, int n)
{
    if (!stringv || n < 0 || n >= stringv->string_count) {
        return NULL;
    }

    return addressof_nth_string(stringv, n);
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
            && dest->block_total >= source->block_used) {
        return copy_bijective(dest, source);
    }

    /* If the destination stringv has a block size greater than or equal to
     * the source's block size, each source string fits in a single
     * source block, and there are at least as many total destination blocks
     * as there are used source blocks, then we don't need to compute the
     * length of each string before copying it into the destination */
    if (dest->block_size >= source->block_size
            && source->block_used == source->string_count
            && dest->block_total >= source->block_used) {
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
    char *block_address = NULL;

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

    block_address = addressof_nth_block(stringv, stringv->block_used);
    memcpy(
            block_address,
            string,
            length);

    stringv->block_used += blocks_required;
    ++stringv->string_count;
    return block_address;
}

char const *stringv_push_front(
        struct stringv *stringv,
        char const *string,
        int length)
{
    int blocks_required = 0;

    /* FIXME: shift_blocks_backward is required for speculative insertion
     * functions that haven't been implemented yet. I write it concurrently
     * with shift_blocks_forward due to their similarity, but strict compiler
     * warnings will flag it as an unused function. So until those insertion
     * routines are implemented, this must stay. */
    (void)shift_blocks_backward;

    if (!stringv || !string || length <= 0) {
        return NULL;
    }

    /* If the stringv is empty, just do a push_back which doesn't involve
     * any shifting. */
    if (stringv->string_count == 0) {
        return stringv_push_back(stringv, string, length);
    }

    blocks_required = blocks_required_by(stringv, length);
    if (stringv->block_used + blocks_required > stringv->block_total) {
        return NULL;
    }

    /* In push_back we would determine the block write address here, but it
     * is fixed as zero as we are inserting to the beginning of the buffer.
     * Instead, we need to shift the blocks forward as appropriate. The shift
     * operation is guarunteed to succeed since we have just confirmed that
     * there is sufficient space. Furthermore, we don't need to save the
     * return address since we know it is simply stringv->buf */
    shift_blocks_forward(stringv, 0, blocks_required);
    memcpy(stringv->buf, string, length);

    /* We need to zero out the remainder of the block(s) that were previously
     * occupied and moved from */
    memset(
            stringv->buf + length,
            0,
            blocks_required * stringv->block_size - length);

    stringv->block_used += blocks_required;
    ++stringv->string_count;
    return stringv->buf;
}

char const *stringv_insert(
        struct stringv *stringv,
        char const *string,
        int length,
        int index)
{
    char *dest = NULL;
    int blocks_required = 0;

    if (!stringv
            || !string
            || length < 0
            || index < 0
            || index > stringv->string_count) {
        return NULL;
    }

    if ((stringv->string_count == 0 && index == 0)
            || index == stringv->string_count) {
        return stringv_push_back(stringv, string, length);
    }

    blocks_required = blocks_required_by(stringv, length);
    if (stringv->block_used + blocks_required > stringv->block_total) {
        return NULL;
    }

    dest = shift_blocks_forward(stringv, index, blocks_required);
    memcpy(dest, string, length);

    memset(
            dest + length,
            0,
            blocks_required * stringv->block_size - length);

    stringv->block_used += blocks_required;
    ++stringv->string_count;
    return dest;
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

    for (; i < source->string_count; ++i) {
        /* Copy the ith string in the source stringv to the nth block
         * in the destination stringv. Since we *know* that each string
         * in the source stringv occupies exactly one block in both the
         * source and destination stringv, we copy source->block_size - 1
         * chars instead of computing the length of each string at each
         * iteration. NUL termination is acheived for free since the copy
         * operation clears the dest stringv. */
        memcpy(
                addressof_nth_block(dest, dest->block_used),
                addressof_nth_string(source, i),
                source->block_size - 1);

        /* All strings occupy one block */
        ++dest->block_used;
        ++dest->string_count;
    }

    /* This function always copies all strings */
    return source->string_count;
}

int copy_iterative(
        struct stringv *dest,
        struct stringv const *source)
{
    int i = 0, ith_string_length = 0, blocks_required = 0, strings_copied = 0;
    char *ith_string = NULL;

    assert(dest);
    assert(source);

    for (; i < source->string_count; ++i) {
        /* Since we don't know before hand whether the source strings will
         * all fit in the destination stringv, we need to compute the length
         * of each string, determine how many blocks it uses, and then check
         * there are at least that many available blocks at each iteration. */
        ith_string = addressof_nth_string(source, i);
        ith_string_length = (int)strlen(ith_string);
        blocks_required = blocks_required_by(dest, ith_string_length);

        /* If there is insufficient room, stop copying */
        if (dest->block_used + blocks_required > dest->block_total) {
            break;
        }

        /* Otherwise, copy the string over */
        memcpy(
                addressof_nth_block(dest, dest->block_used),
                ith_string,
                ith_string_length);

        dest->block_used += blocks_required;
        ++dest->string_count;
        ++strings_copied;
    }

    return strings_copied;
}

static char *shift_blocks_forward(
        struct stringv *stringv,
        int block_index,
        int n)
{
    char *source = NULL, *dest = NULL;
    int size = 0;

    assert(stringv);
    assert(block_index >= 0);
    assert(block_index < stringv->block_used);
    assert(n >= 0);
    assert(n < stringv->block_total);

    source = addressof_nth_block(stringv, block_index);
    if (n == 0) {
        return source;
    }

    dest = source + n * stringv->block_size;
    size = (stringv->block_used - block_index) * stringv->block_size;
    memmove(dest, source, size);
    return source;
}

static char *shift_blocks_backward(
        struct stringv *stringv,
        int block_index,
        int n)
{
    char *start_address = NULL;
    int size = 0;

    assert(stringv);
    assert(block_index >= 0 && block_index < stringv->block_total);
    assert(n >= 0 && n < stringv->block_total);
    assert(block_index - n >= 0);

    start_address = addressof_nth_block(stringv, block_index);
    if (n == 0) {
        return start_address;
    }

    size = stringv->block_size * n;
    memmove(start_address - size, start_address, size);
    return start_address;
}
