#ifndef STRINGV_H_
#define STRINGV_H_

#include <stddef.h>

struct stringv {
    char *buf;          /* Pointer to buffer; mutable */
    size_t size;        /* The size of the buffer (including terminating \0 */
    size_t blocksize;   /* The block size */
    size_t count;       /* The number of strings stored */
};

/* Initialises a stringv, returns 0 on failure */
int stringv_init(
        struct stringv *stringv,    /* stringv object to initialise */
        char *buf,                  /* The buffer to use, must be writeable */
        size_t size,                /* The size of the buffer, in chars */
        size_t blocksize);          /* The desired block size */

#endif /* STRINGV_H_ */
