#include "profile.h"

#include <assert.h>
#include <stdlib.h>
#include <stdio.h>

#define STRINGV_ALLOCATION (4*1024)

/* Returns the size of the given **OPEN** file object. */
static size_t get_file_size(FILE *file);

/* Iterates through the given buffer up to size, and replaces each occurence
 * of the line break character \n with the NUL terminator \0. */
static char *process_linebreaks(char *buf, size_t size);

/* Parses the stats header of a sample file. See generate-strings.py for how
 * the stats header is formatted and generated. */
static int parse_stats(struct profile_sample *sample);

/* Returns a pointer to the next string in the sample's buffer assuming that
 * current is a pointer to the start of the previous string. If there are
 * no more strings, this function returns NULL. */
static char const *next_string(
        struct profile_sample const *sample,
        char const *current);

struct profile_sample *profile_sample_read(
        struct profile_sample *sample,
        char const *filename)
{
    FILE *file = NULL;

    assert(filename);
    file = fopen(filename, "rb");
    assert(file);

    sample->size = get_file_size(file) + 1;
    assert(sample->size > 0);
    sample->buf = calloc(1, sample->size);
    assert(sample->buf);

    fread(sample->buf, 1, sample->size - 1, file);
    fclose(file);

    process_linebreaks(sample->buf, sample->size);
    sample->buf[sample->size] = '\0';

    if (!parse_stats(sample)) {
        return NULL;
    }

    assert(sample->count > 0);
    assert(sample->mean > 0);
    assert(sample->variance >= 0);

    return sample;
}

struct stringv *profile_init_stringv(
        struct profile_sample const *sample,
        struct stringv *stringv,
        int block_size)
{
    char *buf = NULL, *s = NULL;

    assert(sample);
    assert(stringv);

    /* If the sample size is bigger than the allocation parameter
     * STRINGV_ALLOCATION, then even in the best-case storage scenario, the
     * stringv will not be able to store all the sample's strings. */
    if (sample->size > STRINGV_ALLOCATION) {
        return NULL;
    }

    /* Allocate the backing memory for the stringv. */
    buf = calloc(1, STRINGV_ALLOCATION);
    if (!buf) {
        return NULL;
    }

    if (!stringv_init(stringv, buf, STRINGV_ALLOCATION, block_size)) {
        free(buf);
        return NULL;
    }

    /* Now that the stringv is initialised, we need to iteratively copy each
     * string over from the sample's buffer to the stringv by repeated
     * push_backs. */
    for (s = sample->buf; s != NULL; s = next_string(sample, s)) {
        if (!stringv_push_back(stringv, s, strlen(s))) {
            free(buf);
            return NULL;
        }
    }

    return stringv;
}

size_t get_file_size(FILE *file)
{
    size_t size = 0;

    assert(file);
    fseek(file, 0, SEEK_END);
    size = ftell(file);
    rewind(file);

    return size;
}

char *process_linebreaks(char *buf, size_t size)
{
    size_t i = 0;

    assert(buf);
    assert(size > 0);

    for (i = 0; i < size; ++i) {
        if (buf[i] == '\n') {
            buf[i] = '\0';
        }
    }

    return buf;
}

int parse_stats(struct profile_sample *sample)
{
    assert(sample);
    assert(sample->buf);

    /* A correctly formatted stats header will have exactly the 3 numeric
     * parameters described. */
    return (sscanf(sample->buf, "# %d %lf %lf", &sample->count, &sample->mean,
            &sample->variance) == 3);
}

char const *next_string(
        struct profile_sample const *sample,
        char const *current)
{
    assert(sample);
    assert(current);

    while (current != '\0') {
        ++current;
    }

    /* At this point, current points to a NUL terminator. It may be a
     * separating NUL terminator, or it may be the final (string-ending) NUL
     * terminator. We return ++current and NULL for each case respectively. */
    if (current + 1 != sample->buf + size) {
        return ++current;
    } else {
        return NULL;
    }
}
