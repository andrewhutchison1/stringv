#include "stringv.h"

#include <assert.h>
#include <string.h>

typedef int block_pos;
typedef char *block_ptr;

#ifndef NDEBUG

/* Checks that a stringv is valid. */
static int valid_stringv(
        struct stringv const *s);

/* Checks that a block_pos is valid, given the corresponding stringv. */
static int valid_block_pos(
        struct stringv const *s,
        block_pos bn);

/* Checks that a block_ptr is valid, given the corresponding stringv. This
 * only really checks that the pointer is within the buffer of the stringv. */
static int valid_block_ptr(
        struct stringv const *s,
        block_ptr bptr);

#endif /* NDEBUG */

/* Checks that a string_pos is valid, given the corresponding stringv. A
 * string_pos is valid for *reads* if it is in the interval
 * [0, s->string_count). However, a string_pos is valid for *writes* if it is
 * in the interval [0, s->string_count]. */
static int valid_string_pos(
        struct stringv const *s,
        string_pos sn,
        int read);

/* Given a string_pos, returns the corresponding block pos. */
static block_pos string_pos_to_block_pos(
        struct stringv const *s,
        string_pos sn);

/* Given a block_pos, returns the corresponding address of the start of that
 * block. */
static block_ptr block_pos_to_block_ptr(
        struct stringv const *s,
        block_pos bn);

/* Returns a pointer to the string determined by the given string_pos. */
static block_ptr string_pos_to_block_ptr(
        struct stringv const *s,
        string_pos sn);

/* Clears the block range [first, last) by setting each block to binary zero.
 * Returns first. */
static block_pos clear_block_range(
        struct stringv *s,
        block_pos first,
        block_pos last);

/* Determines how many blocks would be required to store the given length
 * of data (in chars; NOT including the NUL terminator) in the given
 * stringv. */
static int blocks_required(
        struct stringv const *s,
        int length);

/* Copies blocks from a source stringv to a destination stringv assuming that
 * source and destination have identical block sizes and destination has
 * the same or a higher block total. Returns the number of strings copied
 * (which is always equal to source->string_count) */
static int copy_blockwise_bijective(
        struct stringv *dest,
        struct stringv const *source);

/* Copies blocks from a source stringv to a destination stringv assuming that
 * the source stringv has as many blocks as strings, and the destination
 * stringv has the same or greater block size. Returns the number of strings
 * copied (which is always equal to source->string_count) */
static int copy_blockwise_injective(
        struct stringv *dest,
        struct stringv const *source);

/* Copies strings from a source stringv to a destination stringv. Returns the
 * number of strings copied, which may be less than the source's string count
 * depending on the destination stringv's block size and block total. */
static int copy_stringwise(
        struct stringv *dest,
        struct stringv const *source);

/* Shifts a range of blocks by the given offset. The offset may be negative
 * (indicating a left shift) or positive (indicating a right shift). In the
 * case of a right shift, the gap created is zeroed. Note that a right shift
 * will leave the stringv in an undefined state (due to internal zero blocks)
 * that must be resolved immediately by writing to this space. For a right
 * shift, the block position of the start of the gap is returned. For a left
 * shift, the block position of the end of the block range is returned. */
static block_pos shift_blocks(
        struct stringv *s,
        block_pos first,
        block_pos last,
        int offset);

/* Writes data to the given block position, returning a pointer to that same
 * block. This function does no bounds checking, nor does it zero the remain-
 * der of the block. Both of these conditions are assumed. */
static block_ptr block_write(
        struct stringv *s,
        char const *string,
        int length,
        block_pos first,
        block_pos last);

/* Begin public functions ****************************************************/

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

    /* FIXME */
    (void)valid_block_ptr;
    (void)blocks_required;

    stringv->buf = buf;
    stringv->block_total = buf_size / block_size;
    stringv->block_size = block_size;
    stringv->block_used = stringv->string_count = 0;
    clear_block_range(stringv, 0, stringv->block_total);
    
    return stringv;
}

char const *stringv_get(
        struct stringv const *stringv,
        string_pos sn)
{
    if (!stringv || !valid_string_pos(stringv, sn, 1)) {
        return NULL;
    }

    assert(valid_stringv(stringv));
    return string_pos_to_block_ptr(stringv, sn);
}

struct stringv *stringv_clear(
        struct stringv *stringv)
{
    if (stringv) {
        assert(valid_stringv(stringv));
        clear_block_range(stringv, 0, stringv->block_total);
        stringv->block_used = stringv->string_count = 0;
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

    assert(valid_stringv(dest));
    assert(valid_stringv(source));

    /* The destination needs to be cleared. */
    stringv_clear(dest);

    /* If the source stringv is empty, than we don't need to do anything. */
    if (source->string_count == 0) {
        return 0;
    }

    /* If the destination stringv has the same block size, then we can just
     * memcpy the entire block over if it also has a greater than or equal
     * block total. */
    if (dest->block_size == source->block_size
            && dest->block_total >= source->block_used) {
        return copy_blockwise_bijective(dest, source);
    }

    /* If the destination stringv has a block size greater than or equal to the
     * source's block size, each source string fits in a single source block,
     * and there are at least as many total destination blocks as there are
     * used source blocks, then we don't need to compute the length of each
     * string before copying, as we know all strings will fit. */
    if (dest->block_size >= source->block_size
            && source->block_used == source->string_count
            && dest->block_total >= source->block_used) {
        return copy_blockwise_injective(dest, source);
    }

    return copy_stringwise(dest, source);
}


char const *stringv_push_back(
        struct stringv *stringv,
        char const *string,
        int length)
{
    int blocks_req = 0;

    if (!stringv || !string || length <= 0) {
        return NULL;
    }

    assert(valid_stringv(stringv));

    /* push_back can't be implemented in terms of insert because insert defers
     * to push_back as a special case. Indeed, push_back is far simpler to
     * implement than insert with the only common code being the block
     * capacity check. */
    blocks_req = blocks_required(stringv, length);
    if (stringv->block_used + blocks_req > stringv->block_total) {
        return NULL;
    }

    /* If there is enough room, all we need to do is write to the end of the
     * stringv and bump the string and block counts as appropriate. */
    return block_write(
            stringv,
            string,
            length,
            stringv->block_used,
            stringv->block_used + blocks_req);
}

char const *stringv_push_front(
        struct stringv *stringv,
        char const *string,
        int length)
{
    return stringv_insert(stringv, string, length, 0);
}

char const *stringv_insert(
        struct stringv *stringv,
        char const *string,
        int length,
        string_pos sn)
{
    int blocks_req = 0;
    block_pos write_pos = 0;

    if (!stringv
            || !string
            || length <= 0
            || !valid_string_pos(stringv, sn, 0)) {
        return NULL;
    }

    assert(valid_stringv(stringv));

    /* An insertion at the end of the stringv is equivalent to a push_back.
     * This also handles insertions into an empty stringv */
    if (sn == stringv->string_count) {
        return stringv_push_back(stringv, string, length);
    }

    /* Ensure there is sufficient room in the stringv */
    blocks_req = blocks_required(stringv, length);
    if (stringv->block_used + blocks_req > stringv->block_total) {
        return NULL;
    }

    assert(blocks_req != 0);

    /* We know that the insertion is occurring internally, so we will need
     * to shift blocks *right* to accomodate this. We also know that there
     * is sufficient space to do so. */
    write_pos = shift_blocks(
            stringv,
            string_pos_to_block_pos(stringv, sn),
            stringv->block_used,
            blocks_req);

    return block_write(
            stringv,
            string,
            length,
            write_pos,
            write_pos + blocks_req);
}

/* End public functions ******************************************************/

#ifndef NDEBUG

int valid_stringv(
        struct stringv const *s)
{
    return s
        && s->buf
        && s->block_total > 0
        && s->block_size > 0
        && s->block_used >= 0 && s->block_used <= s->block_total
        && s->string_count >= 0 && s->string_count <= s->block_total;
}

int valid_block_pos(
        struct stringv const *s,
        block_pos bn)
{
    /* A block pos is valid if it is in the interval [0, s->block_total]. The
     * right end of the interval includes the block total since block positions
     * can be used as ranges, and we want to allow one-past-the-end positions
     * to make range arithmetic simpler. */
    return  bn >= 0 && bn <= s->block_total;
}

int valid_block_ptr(
        struct stringv const *s,
        block_ptr bptr)
{
    return bptr >= s->buf
        && bptr < (s->buf + s->block_size * (s->block_total - 1));
}

#endif /* NDEBUG */

int valid_string_pos(
        struct stringv const *s,
        string_pos sn,
        int read)
{
    return sn >= 0 && (read ? sn < s->string_count : sn <= s->string_count);
}

block_pos string_pos_to_block_pos(
        struct stringv const *s,
        string_pos sn)
{
    block_pos i = 0;

    assert(s && valid_stringv(s));
    assert(valid_string_pos(s, sn, 1));

    /* The first string always starts at the start of the stringv's buffer. */
    if (sn == 0) {
        return 0;
    }

    /* If the stringv's string count is equal to the number of used blocks,
     * then the block position corresponding to any string position is simply
     * that string position. */
    if (s->string_count == s->block_used) {
        return sn;
    }

    /* Otherwise, we need to iterate through each block in the stringv, and
     * check that the character immediately preceding the start of each block
     * is NUL. If so, then the start of that block also starts a string. */
    for (i = 0; sn > 0 && i < s->block_total; ++i) {
        /* If the preceding is NUL, we found a string. */
        if (!s->buf[(i + 1) * s->block_size - 1]) {
            --sn;
        }
    }

    assert(sn == 0);
    return i;
}

block_ptr block_pos_to_block_ptr(
        struct stringv const *s,
        block_pos bn)
{
    assert(s && valid_stringv(s));
    assert(valid_block_pos(s, bn));
    return s->buf + bn * s->block_size;
}

block_ptr string_pos_to_block_ptr(
        struct stringv const *s,
        string_pos sn)
{
    return block_pos_to_block_ptr(
            s,
            string_pos_to_block_pos(s, sn));
}

block_pos clear_block_range(
        struct stringv *s,
        block_pos first,
        block_pos last)
{
    assert(s && valid_stringv(s));
    assert(valid_block_pos(s, first));
    assert(valid_block_pos(s, last) || last == s->block_total);

    memset(block_pos_to_block_ptr(s, first),
            0,
            (last - first) * s->block_size);

    return first;
}

int blocks_required(
        struct stringv const *s,
        int length)
{
    assert(s && valid_stringv(s));
    assert(length > 0);

    /* Increment the length to accomodate the NUL character. All strings
     * in a stringv are NUL terminated (with at least one NUL terminator, 
     * by design. */
    ++length;

    return (length / s->block_size) + (length % s->block_size != 0);
}

int copy_blockwise_bijective(
        struct stringv *dest,
        struct stringv const *source)
{
    assert(dest && valid_stringv(dest));
    assert(source && valid_stringv(source));
    assert(source->block_size == dest->block_size);
    assert(source->block_used <= dest->block_total);

    memcpy(dest->buf, source->buf, source->block_size * source->block_used);
    dest->string_count = source->string_count;
    dest->block_used = source->block_used;
    return dest->string_count;
}

int copy_blockwise_injective(
        struct stringv *dest,
        struct stringv const *source)
{
    string_pos i = 0;

    assert(dest && valid_stringv(dest));
    assert(source && valid_stringv(source));
    assert(source->block_used == source->string_count);
    assert(source->block_size <= dest->block_size);

    for (i = 0; i < source->string_count; ++i) {
        /* We know that each string in the source stringv occupies exactly
         * one block in the destination stringv. So we copy each string
         * over to each block. Instead of computing the strings length with
         * strlen, we just copy source->block_size - 1 characters since the
         * string lengths are bounded by this value. */
        memcpy(
                block_pos_to_block_ptr(dest, i),
                block_pos_to_block_ptr(source, i),
                source->block_size - 1);
    }

    dest->string_count = dest->block_used = source->string_count;
    return dest->string_count;
}

int copy_stringwise(
        struct stringv *dest,
        struct stringv const *source)
{
    string_pos i = 0;
    int ith_length = 0, blocks_req = 0;
    char const *ith_string = NULL;

    assert(dest && valid_stringv(dest));
    assert(source && valid_stringv(source));

    for (i = 0; i < source->string_count; ++i) {
        /* We don't know whether the source string will fit in the destination
         * stringv, so we need to compute the length of each string,
         * determine how many blocks it uses, and bail out if the required
         * number of blocks is greater than the available blocks in dest. */
        ith_string = string_pos_to_block_ptr(source, i);
        ith_length = (int)strlen(ith_string);
        blocks_req = blocks_required(dest, ith_length);

        /* If there is insufficient room, stop copying */
        if (dest->block_used + blocks_req > dest->block_total) { break; }

        /* Otherwise, copy the string over */
        memcpy(
                block_pos_to_block_ptr(dest, dest->block_used),
                ith_string,
                ith_length);

        /* And increment the string and block counters */
        dest->block_used += blocks_req;
        ++dest->string_count;
    }

    return dest->string_count;
}

block_pos shift_blocks(
        struct stringv *s,
        block_pos first,
        block_pos last,
        int offset)
{
    assert(s && valid_stringv(s));
    assert(valid_block_pos(s, first));
    assert(valid_block_pos(s, last));
    assert(last > first);
    assert(offset != 0);

    memmove(
            block_pos_to_block_ptr(s, first + offset),
            block_pos_to_block_ptr(s, first),
            (last - first) * s->block_size);

    if (offset < 0) {
        /* If the offset is negative, then the shift is a left shift.
         * We return the block position of the end of the shifted range. */
        return last + offset;
    } else {
        /* Otherwise, the offset is positive and the shift is a right shift.
         * We return the block position of the start of the gap that we just
         * created from the shift after zeroing it out. */
        clear_block_range(s, first, first + offset);
        return first;
    }
}

block_ptr block_write(
        struct stringv *s,
        char const *string,
        int length,
        block_pos first,
        block_pos last)
{
    block_ptr write_ptr = NULL;

    assert(s && valid_stringv(s));
    assert(string);
    assert(length > 0);
    assert(valid_block_pos(s, first));
    assert(valid_block_pos(s, last));
    assert(last > first);

    write_ptr = memcpy(block_pos_to_block_ptr(s, first), string, length);
    s->block_used += (last - first);
    ++s->string_count;

    return write_ptr;
}
