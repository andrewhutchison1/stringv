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

/* Copies a stringv to another stringv. Returns 1 on success and 0 on failure.
 * You can use this to copy the contents of a full stringv to another stringv
 * with a larger buffer. If the function fails, no work is done */
int stringv_copy(
        struct stringv *dest,       /* stringv to copy to */
        struct stringv *source);    /* stringv to copy from */

/* Pushes a string to the end of the stringv. Returns 1 on success, 0 on
 * failure. The index of the newly created string element can be recovered
 * through stringv->string_count - 1. */
int stringv_push_back(
        struct stringv *stringv,
        char const *string,
        unsigned string_length);


#endif /* STRINGV_H_ */
