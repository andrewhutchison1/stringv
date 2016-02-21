#ifndef STRINGV_H_
#define STRINGV_H_

enum stringv_error {
    stringv_success,
    stringv_invalid_argument,
    stringv_block_size_mismatch,
    stringv_insufficient_blocks
};

struct stringv {
    char *buf;
    unsigned block_total;
    unsigned block_size;
    unsigned block_used;
    unsigned string_count;
};

/* Initialises a stringv, returns 0 on failure */
int stringv_init(
        struct stringv *stringv,    /* stringv object to initialise */
        char *buf,                  /* The buffer to use, must be writeable */
        unsigned buf_size,          /* The size of the buffer, in chars */
        unsigned block_size);       /* The desired block size */

/* Copies the data stored in the source stringv to the destination stringv.
 * Preserves the block size of the destination stringv. Returns dest */
struct stringv *stringv_copy(
        struct stringv *dest,       /* stringv to copy to */
        struct stringv *source,     /* stringv to copy from */
        enum stringv_error *error);

/* Pushes a string to the end of the stringv. Returns 1 on success, 0 on
 * failure. The index of the newly created string element can be recovered
 * through stringv->string_count - 1. */
int stringv_push_back(
        struct stringv *stringv,
        char const *string,
        unsigned string_length,
        enum stringv_error *error);

#endif /* STRINGV_H_ */
