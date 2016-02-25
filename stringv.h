#ifndef STRINGV_H_
#define STRINGV_H_

struct stringv {
    char *buf;
    int block_total;
    int block_size;
    int block_used;
    int string_count;
};

/* Initialises a stringv to an initial valid (but empty) state with the
 * given block size.
 *
 *      buf         A pointer to a writable buffer which will serve as the
 *                  storage for the stringv. buf must be NUL terminated.
 *      buf_size    The size of buf, in chars, including the terminating
 *                  character.
 *      block_size  The desired size of the blocks. The block size is measured
 *                  in characters and includes the NUL terminator.
 *                  The block size should match the expected median length
 *                  of the stored strings.
 *
 * PRE:     stringv != NULL
 *          buf != NULL
 *          buf_size > 1
 *          1 < block_size <= buf_size
 *          buf[buf_size] == '\0'
 *
 * POST:    stringv->block_used == stringv->string_count == 0
 *          stringv->buf = {0, ..., 0}
 */
struct stringv *stringv_init(
        struct stringv *stringv,
        char *buf,
        int buf_size,
        int block_size);

/* Clears a stringv by zeroing the buffer and resetting the string count
 * and used block count.
 *
 *      stringv     The stringv to clear. If this parameter is NULL then no
 *                  work is done.
 *
 * PRE:     (stringv != NULL) ==> (stringv->buf != NULL)
 *
 * POST:    (stringv != NULL) ==> (stringv->buf == {0, ..., 0})
 *                             && (stringv->block_used == 0)
 *                             && (stringv->string_count == 0)
 */
void stringv_clear(struct stringv *stringv);

/* Copies the data stored in the source stringv to the destination stringv.
 * Preserves the block size of the destination stringv. Returns NULL if
 * invalid arguments are given or there is insufficient space to copy the
 * data. Otherwise dest is returned.
 *
 *      dest        The stringv to write to. dest is cleared as if through
 *                  a call to stringv_clear (its block size is retained).
 *      source      The stringv to read from.
 *
 * PRE:     dest != NULL
 *          source != NULL
 *
 * POST:    source unchanged
 *          dest->block_size unchanged
 *          dest->string_count = source->string_count
 */
struct stringv *stringv_copy(
        struct stringv *dest,
        struct stringv *source);

/* Appends a string to the stringv, returning a pointer to its location,
 * or NULL on error. The index of the appended string can be recovered
 * through stringv->string_count - 1 */
char const *stringv_push_back(
        struct stringv *stringv,
        char const *string,
        int string_length);

#endif /* STRINGV_H_ */
