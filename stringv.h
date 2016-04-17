#ifndef STRINGV_H_
#define STRINGV_H_

#include <stddef.h>

#if defined(__cplusplus)
extern "C" {
#endif /* defined(__cplusplus) */

#if defined(__STDC__) && defined(__STD_VERSION__) && __STD_VERSION__ >= 199901
#   define STRINGV_RESTRICT     restrict
#   define STRINGV_ZERO         (struct stringv){NULL,0,0,0,0}
#else
#   define STRINGV_RESTRICT     /* Nothing */
#   define STRINGV_ZERO         {NULL,0,0,0,0}
#endif /* defined(__STDC__) && ... */

struct stringv {
    char *buf;
    int block_total;
    int block_size;
    int block_used;
    int string_count;
};

/* Convenience macro for the zero-initialization of a stringv object. The
 * expression this macro expands to is a rvalue that should only appear
 * on the right side of an assignment expression where the left side if of
 * type struct stringv. */
#if defined(__STDC__) && defined(__STD_VERSION__) && __STD_VERSION__ >= 199901
#   define STRINGV_ZERO (struct stringv){NULL,0,0,0,0}
#else
#   define STRINGV_ZERO {NULL,0,0,0,0}
#endif /* defined(__STDC__) && ... */

/* The string_pos is an integral quantity that determines (uniquely) a
 * specific string inside a stringv. */
typedef int string_pos;

/* Comparison function used to compare strings in a stringv. Currently only
 * used by stringv_sort. The function should have identical semantics to
 * strnlen: returning 0 if the strings are equal, or negative/positive
 * values if the first string is lexicographically lesser or greater than
 * the second respectively. */
typedef int (*lexicographical_compare)(char const *, char const *, size_t);

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

/* Retrieves the string at the nth position in the stringv. If any of the
 * arguments are invalid or the index is out of range, the function will
 * return NULL. stringv_get can be used to iterate, but it is inefficient
 * since stringv_get must count blocks from the beginning each time. For
 * iteration, use stringv_begin/stringv_next/stringv_end.
 *
 *      stringv     The stringv to read from.
 *      n           The position, starting from 0, that determines which
 *                  string we want to retrieve.
 *      RETURNS     A non-writable pointer to the string indexed by n. If
 *                  stringv is NULL or n is out of range, then the return
 *                  value will be NULL.
 *
 *      PRE:        stringv != NULL
 *                  0 <= n < stringv->string_count
 *      POST:       stringv unchanged
 */
char const *stringv_get(struct stringv const *stringv, int n);

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
 *                  Iterators invalidated
 */
struct stringv *stringv_clear(struct stringv *stringv);

/* Attempts to copy the strings stored in the source stringv to the dest-
 * ination stringv. The destination stringv is cleared before copying, and
 * its block size is preserved. Since the block sizes of the source and
 * destination stringv may be different, and their sizes are fixed, it may
 * not be possible to store all strings from the source stringv in the
 * destination stringv. stringv_copy will copy as many strings as possible,
 * and return the number of strings copied.
 *
 *      dest        The stringv to write to. dest is cleared as if through
 *                  a call to stringv_clear (its block size is retained).
 *      source      The stringv to read from.
 *      RETURNS     The number of strings copied.
 *
 *      PRE:        dest != NULL
 *                  source != NULL
 *                  dest != source
 *      POST:       source unchanged
 *                  dest->block_size unchanged
 *                  dest->string_count <= source->string_count
 */
int stringv_copy(
        struct stringv *STRINGV_RESTRICT dest,
        struct stringv const *STRINGV_RESTRICT source);

/* Appends a string to the stringv, returning a pointer to its location,
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
 *                  function will fail when the arguments are invalid (see
 *                  preconditions) or there is insufficient space for writing
 *                  the string.
 *
 *      PRE:        stringv != NULL
 *                  string != NULL
 *                  length > 0
 *      POST:       stringv->block_used increased
 *                  stringv->string_count incremented
 *                  return pointer == &(stringv->string_count - 1)th string
 *                  Iterators invalidated
 */
char const *stringv_push_back(
        struct stringv *STRINGV_RESTRICT stringv,
        char const *STRINGV_RESTRICT string,
        int length);

/* Prepends a string to the stringv, returning a pointer to its location, or
 * NULL on error. If the function succeeds, the index of the prepended string
 * will be zero. Like stringv_push_back, the returned pointer is const so that
 * client code doesn't have a chance of overwriting the block boundary, hence
 * violating the stringv invariants. If the function fails, no external state
 * is modified.
 *
 *      stringv     The stringv to write to.
 *      string      The string to write to the stringv. NUL termination is not
 *                  required.
 *      length      The length of the string, or how many chars to write to the
 *                  stringv.
 *      RETURNS     A non-writable pointer on success, or NULL on failure. The
 *                  function will fail when the preconditions are not satisfied
 *                  or there is insufficient space in the stringv.
 *
 *      PRE:        stringv != NULL
 *                  string != NULL
 *                  length > 0
 *      POST:       stringv->block_used increased
 *                  stringv->string_count incremented
 *                  return pointer == &(0th string) == &stringv->buf[0]
 *                  Iterators invalidated
 */
char const *stringv_push_front(
        struct stringv *STRINGV_RESTRICT stringv,
        char const *STRINGV_RESTRICT string,
        int length);

/* Inserts a string at the specified string index, returning a pointer to it,
 * or NULL on error. If the function succeeds, the string will be written into
 * the buffer managed by the stringv object such that its index will be the
 * index as given by the corresponding function argument. This function will
 * fail for invalid arguments or insufficient space. If the function fails,
 * no external state is modified.
 *
 *      stringv     The stringv to write to.
 *      string      The string to write to the stringv. NUL termination is not
 *                  required.
 *      length      The length of the string, or how many chars to write to the
 *                  stringv.
 *      RETURNS     A non-writable pointer to the written string on success,
 *                  or NULL on failure.
 *
 *      PRE:        stringv != NULL
 *                  string != NULL
 *                  length > 0
 *                  index >= 0 && index < stringv->string_count
 *      POST:       stringv_get(stringv, index) == string
 *                  Iterators invalidated
 */
char const *stringv_insert(
        struct stringv *STRINGV_RESTRICT stringv,
        char const *STRINGV_RESTRICT string,
        int length,
        int index);

/* Inserts into a stringv each substring of the given string, up to length,
 * where each substring is delimited by the given separator character. The
 * string is inserted as if a call to string_push_back was made. If there is
 * insufficient space to store a substring, the stringv will retain all
 * previous insertions.
 *
 *      stringv     The stringv to write to.
 *      string      The string to split.
 *      length      The length of the argument string.
 *      separator   The separator (delimiting) character.
 *      RETURNS     The index of the first non-delimiter character that was
 *                  not read into the stringv if there is insufficient space,
 *                  or length otherwise.
 *
 *      PRE:        stringv != NULL
 *                  string != NULL
 *                  length > 0
 *      POST:       stringv data not overwritten or deleted
 */
int stringv_split(
        struct stringv *STRINGV_RESTRICT stringv,
        char const *STRINGV_RESTRICT string,
        int length,
        char separator);

/* Removes the string specified by the index argument from the stringv.
 * Returns 1 on success or 0 on failure, occuring when the arguments are
 * invalid or the index is out of range
 *
 *      stringv     The stringv to remove from.
 *      sn          The position of the string to remove
 *      RETURNS     1 on success, 0 on failure
 *
 *      PRE:        stringv != NULL
 *                  index >= 0 && index < stringv->string_count
 *      POST:       stringv->string_count decremented
 *                  stringv->block_used decreased
 *                  Iterators invalidated
 */
int stringv_remove(struct stringv *stringv, string_pos sn);

/* Returns the address of the first string in the stringv suitable for
 * iteration.
 *
 *      stringv     The stringv to begin iteration on
 *      RETURNS     The pointer to the first string in the stringv, suitable
 *                  for continued iteration with stringv_next and
 *                  stringv_end.
 *
 *      PRE:        stringv is a valid stringv
 *      POST:       stringv_begin(stringv) == stringv_get(stringv, 0)
 *                  stringv unmodified
 */
char const *stringv_begin(struct stringv const *stringv);

/* Returns the address of the one-past-the-end byte of the stringv's buffer.
 * The pointer to be used should only be used in a comparison expression to
 * stop iteration. Dereferencing this pointer is undefined behaviour.
 *
 *      stringv     The stringv to return the end pointer from.
 *      RETURNS     A pointer to the one-past-the-end byte of the given
 *                  stringv's buffer.
 *
 *      PRE:        stringv is a valid stringv
 *      POST:       stringv unmodified
 */
char const *stringv_end(struct stringv const *stringv);

/* Given a stringv and an iterator (pointer to const char), string_next
 * computes the address of string succeeding the given iterator. Dereferencing
 * the pointer returned by stringv_next is undefined behaviour if the
 * address lies beyond the stringv's buffer. When calling stringv_next in an
 * iterative context, a comparison with stringv_end should be made at each
 * iteration in order to avoid invoking undefined behaviour.
 *
 *      stringv     The stringv to iterate upon.
 *      iter        The iterator to return the next iterator to.
 *
 *      PRE:        stringv is a valid stringv
 *                  iter >= stringv->buf && iter < stringv->buf
 *                  iter points to the start of a block
 *      POST:       stringv unmodified
 *                  returned pointer <= stringv_end
 *                  returned pointer p dereferenceable iff p != stringv_end
 */
char const *stringv_next(
        struct stringv const *STRINGV_RESTRICT stringv,
        char const *STRINGV_RESTRICT iter);

#if defined(__cplusplus)
}
#endif /* defined(__cplusplus) */

#endif /* STRINGV_H_ */
