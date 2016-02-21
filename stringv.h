#ifndef STRINGV_H_
#define STRINGV_H_

struct stringv {
    char *buf;          /* Pointer to buffer; mutable */
    unsigned size;        /* The size of the buffer (including terminating \0 */
    unsigned blocksize;   /* The block size */
    unsigned blockused;   /* The number of used blocks */
    unsigned count;       /* The number of strings stored */
};

/* Initialises a stringv, returns 0 on failure */
int stringv_init(
        struct stringv *stringv,    /* stringv object to initialise */
        char *buf,                  /* The buffer to use, must be writeable */
        unsigned size,                /* The size of the buffer, in chars */
        unsigned blocksize);          /* The desired block size */

/* Copies a stringv to another stringv. Returns 1 on success and 0 on failure.
 * You can use this to copy the contents of a full stringv to another stringv
 * with a larger buffer. If the function fails, no work is done */
int stringv_copy(
        struct stringv *dest,
        struct stringv *source);

#endif /* STRINGV_H_ */
