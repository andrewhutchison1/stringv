#include "bench.h"

#include <assert.h>
#include <string.h>
#include <stdlib.h>

char const *read_strings(char const *filename)
{
    FILE *file = NULL;
    char *content = NULL;
    size_t size = 0;

    /* Try to open the file */
    assert(filename);
    file = fopen(filename, "rb");
    assert(file);

    /* Try to determine the file size */
    fseek(file, 0, SEEK_END);
    size = ftell(file);
    rewind(file);

    /* Try to allocate a buffer to store the file contents */
    content = calloc(size + 1, 1);
    assert(content);

    /* Read the file data into the buffer */
    fread(content, 1, size, file);
    fclose(file);

    return content;
}
