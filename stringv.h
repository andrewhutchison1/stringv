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
 * given block size. If the function succeeds, a pointer to an initialised
 * stringv is returned. If the function fails, no external state is
 * modified.
 *
 *      stringv     A pointer to an uninitialised stringv.
 *      buf         A pointer to a writable buffer which will serve as the
 *                  storage for the stringv. buf must be NUL terminated.
 *      buf_size    The size of buf, in chars, including the terminating
 *                  character.
 *      block_size  The desired size of the blocks. The block size is
 *                  measured in characters and includes the NUL terminator.
 *                  The block size should match the expected median length
 *                  of the stored strings.
 *      RETURNS     stringv on success, NULL on failure.
 *
 *      PRE:        stringv != NULL
 *                  buf != NULL
 *                  buf_size > 1
 *                  1 < block_size <= buf_size
 *                  buf[buf_size] == '\0'
 *      POST:       stringv->block_used == stringv->string_count == 0
 *                  stringv->buf = {0, ..., 0}
 */
struct stringv *stringv_init(
        struct stringv *stringv,
        char *buf,
        int buf_size,
        int block_size);

/* Clears a stringv by zeroing the buffer and resetting the string count
 * and used block count. Returns its argument.
 *
 *      stringv     The stringv to clear. If this parameter is NULL then no
 *                  work is done.
 *      RETURNS     stringv
 *
 *      PRE:        (stringv != NULL) ==> (stringv->buf != NULL)
 *      POST:       (stringv != NULL) ==> (stringv->buf == {0, ..., 0})
 *                          && (stringv->block_used == 0)
 *                          && (stringv->string_count == 0)
 */
struct stringv *stringv_clear(struct stringv *stringv);

/* Copies the data stored in the source stringv to the destination stringv.
 * Preserves the block size of the destination stringv. If the function
 * fails, no external state is modified.
 *
 *      dest        The stringv to write to. dest is cleared as if through
 *                  a call to stringv_clear (its block size is retained).
 *      source      The stringv to read from.
 *      RETURNS     A pointer to dest if the the function succeeds,
 *                  NULL otherwise.
 *
 *      PRE:        dest != NULL
 *                  source != NULL
 *      POST:       source unchanged
 *                  dest->block_size unchanged
 *                  dest->string_count = source->string_count
 */
struct stringv *stringv_copy(
        struct stringv *dest,
        struct stringv *source);

/* Appends a string to the stringv, returning a pointer to its loctions,
 * or NULL on error. The index of the appended string can be recovered
 * immediately through stringv->string_count - 1. The pointer returned
 * is const, since overwriting the string's blocks may violate the
 * invariants of stringv. Unsafe insertion operations (that return mutable
 * pointers) are provided. If the function fails, no external state is
 * modified.
 *
 *      stringv     The stringv to write to.
 *      string      The string to write to the stringv. string doesn't
 *                  actually need to be NUL terminated.
 *      length      The length of the string. If the string is NUL termin-
 *                  ated, this can be the (casted) result of strlen.
 *      RETURNS     A non-writable pointer to the written string in the
 *                  stringv if successful, otherwise NULL is returned. The
 *                  function can fail when the arguments are invalid (see
 *                  preconditions) or there is insufficient space for writing
 *                  the string.
 *
 *      PRE:        stringv != NULL
 *                  string != NULL
 *                  length > 0
 *      POST:       stringv->block_used increased
 *                  stringv->string_count incremented
 *                  return pointer == &(stringv->string_count - 1)th string
 */
char const *stringv_push_back(
        struct stringv *stringv,
        char const *string,
        int length);

#endif /* STRINGV_H_ */
