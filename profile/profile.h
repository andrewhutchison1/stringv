#ifndef PROFILE_H_
#define PROFILE_H_

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <stddef.h>

/* Defines some shared functions for use in profiling and benchmarking
 * stringv code. */

#define STRINGV_ALLOCATION  (4*1024*1024)
#define RAND_SEED           0xDEADBEEF

/* The profile sample object stores the raw string data contiguously. */
struct profile_sample {
    char *buf;
    size_t size;
    int count;
};

/* Reads a profile sample from a file */
struct profile_sample *profile_sample_read(
        struct profile_sample *sample,
        char const *filename);

/* Returns a pseudo-uniformly distributed integer in the range [low, high). */
int rand_range(int low, int high);

#endif /* PROFILE_H_ */
