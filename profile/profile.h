#ifndef PROFILE_H_
#define PROFILE_H_

#include <stdio.h>
#include <time.h>

#define ALLOCATION_SIZE (4*1024*1024)

struct string_buf {
    char *buf;
    int size;
};

/* Initialises a stringv with the given block size and a 'big enough'
 * buffer */
struct stringv *init_stringv(struct stringv *s, int block_size);

/* Fills a stringv with data from a string_buf */
struct stringv *fill_stringv(struct stringv *s, struct string_buf buf);

/* Reads strings from the given file into a dynamically allocated buffer. */
struct string_buf read_strings(char const *filename);

/* Given a buffer full of strings separated by newlines, replace each newline
 * character with the NUL terminator \0 */
struct string_buf terminate_at_newline(struct string_buf sbuf);

/* Reports the profile results TODO json */
void report_profile_results(
        struct stringv const *s,
        struct string_buf const sbuf,
        clock_t clock);

#endif /* PROFILE_H_ */
