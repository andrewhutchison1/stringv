#ifndef PROFILE_H_
#define PROFILE_H_

#include <stddef.h>

#include "../stringv.h"

/* Defines some shared functions for use in profiling and benchmarking
 * stringv code. */

/* A profile sample is a structure that stores a buffer of strings
 * separated by NUL characters, the number of strings, as well as statistical
 * information regarding the distribution of the string lengths. The strings
 * are assumed to contain only printable characters. */
struct profile_sample {
    size_t size;
    double mean;
    double variance;
    char *buf;
    int count;
};

#define PROFILE_SAMPLE_ZERO {0,0.,0.,NULL,0}

/* Reads a profile sample from a file. The profile sample should be generated
 * from generate-strings.py. The sample parameter can be a pointer to an
 * uninitialised profile_sample object but it must not be NULL. */
struct profile_sample *profile_sample_read(
        struct profile_sample *sample,
        char const *filename);

/* Fills a stringv with the strings from a profile_sample. The stringv must be
 * uninitialized. The stringv is initialized with the given block size. */
struct stringv *profile_init_stringv(
        struct profile_sample const *sample,
        struct stringv *stringv,
        int block_size);

#endif /* PROFILE_H_ */
