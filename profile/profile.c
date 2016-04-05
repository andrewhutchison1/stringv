#include "profile.h"

#include <assert.h>
#include <stdlib.h>
#include <stdio.h>

static size_t get_file_size(FILE *file);
static char *process_linebreaks(char *buf, size_t size);
static int parse_stats(struct profile_sample *sample);

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
