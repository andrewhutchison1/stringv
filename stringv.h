#ifndef STRINGV_H_
#define STRINGV_H_

enum stringv_error {
    stringv_invalid_argument,
    stringv_block_size_mismatch,
    stringv_insufficient_blocks
};

struct stringv {
    char *buf;
    int block_total;
    int block_size;
    int block_used;
    int string_count;
};

/* Initialises a stringv, returns a pointer to stringv, or NULL on error */
struct stringv *stringv_init(
        struct stringv *stringv,    /* stringv object to initialise */
        char *buf,                  /* The buffer to use, must be writeable */
        int buf_size,               /* The size of the buffer, in chars */
        int block_size);            /* The desired block size */

/* Clears a stringv by zeroing the buffer and resetting the string count
 * and used block count. */
void stringv_clear(struct stringv *stringv);

/* Copies the data stored in the source stringv to the destination stringv.
 * Preserves the block size of the destination stringv. */
struct stringv *stringv_copy(
        struct stringv *dest,       /* stringv to copy to */
        struct stringv *source,     /* stringv to copy from */
        enum stringv_error *error);

/* Appends a string to the stringv, returning a pointer to its location,
 * or NULL on error. The index of the appended string can be recovered
 * through stringv->string_count - 1 */
char const *stringv_push_back(
        struct stringv *stringv,
        char const *string,
        int string_length,
        enum stringv_error *error);

#endif /* STRINGV_H_ */
