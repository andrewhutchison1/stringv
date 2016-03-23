#include "profile.h"

#include <assert.h>
#include <string.h>
#include <stdlib.h>

#include "../stringv.h"

struct stringv *init_stringv(struct stringv *s, int block_size)
{
    char *data = calloc(ALLOCATION_SIZE, 1);
    assert(data);
    assert(s);

    return stringv_init(s, data, ALLOCATION_SIZE, block_size);
}

struct stringv *fill_stringv(struct stringv *s, struct string_buf buf)
{
    char const *begin = NULL;
    int i = 0;
    
    assert(s);
    assert(buf.buf && buf.size > 0);

    begin = buf.buf;
    for (i = 0; i < buf.size; ++i) {
        if (!buf.buf[i]) {
            stringv_push_back(s, begin, (buf.buf + i) - begin);
            begin = buf.buf + i + 1;
        }
    }

    return s;
}

struct string_buf read_strings(char const *filename)
{
    struct string_buf sbuf = {NULL, 0};
    FILE *file = NULL;

    /* Try to open the file */
    assert(filename);
    file = fopen(filename, "rb");
    assert(file);

    /* Try to determine the file size */
    fseek(file, 0, SEEK_END);
    sbuf.size = ftell(file) + 1;
    rewind(file);

    /* Try to allocate a buffer to store the file contents */
    sbuf.buf = calloc(sbuf.size, 1);
    assert(sbuf.buf);

    /* Read the file data into the buffer */
    fread(sbuf.buf, 1, sbuf.size - 1, file);
    fclose(file);

    /* NUL terminate */
    sbuf.buf[sbuf.size] = '\0';

    return sbuf;
}

struct string_buf terminate_at_newline(struct string_buf sbuf)
{
    int i = 0;
    assert(sbuf.buf);

    for (i = 0; i < sbuf.size; ++i) {
        if (sbuf.buf[i] == '\n') {
            sbuf.buf[i] = '\0';
        }
    }

    return sbuf;
}

void report_profile_results(
        struct stringv const *s,
        struct string_buf const sbuf,
        clock_t clock)
{
    assert(s);

    printf("Profile completed:\n\tTime %lu\n\tBlock size %d\n\tBlock used "
            "%d/%d\n\tData size %d\n", clock, s->block_size,
            s->block_used, s->block_total, sbuf.size);
}
