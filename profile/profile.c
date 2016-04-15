#include "profile.h"

#include <assert.h>
#include <stdlib.h>
#include <stdio.h>

/* Returns the size of the given **OPEN** file object. */
static size_t get_file_size(FILE *file);

/* Counts the number of lines (strings) in the given NUL terminated string */
static int count_lines(char const *string);

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
    sample->buf[sample->size] = '\0';
    sample->count = count_lines(sample->buf);
    fclose(file);

    return sample;
}

int rand_range(int low, int high)
{
    int r = 0;

    return low + (rand() % (int)(high - low + 1));
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

int count_lines(char const *string)
{
    assert(string);
    int i = 0, lines = 0;

    while (string[i]) {
        if (string[i++] == '\n') {
            ++lines;
        }
    }

    return lines;
}
