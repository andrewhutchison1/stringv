#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "profile.h"
#include "../stringv.h"

static clock_t profile(struct stringv *s);

int main(int argc, char **argv)
{
    struct stringv s = {0};
    struct string_buf sbuf;
    char const *filename = NULL;
    int block_size = 0;
    clock_t result;

    /* The command line arguments are (1) the block size of the stringv,
     * and (2) the filename of the random string file */
    assert(argc == 3);

    block_size = atoi(argv[1]);
    assert(block_size > 0);
    filename = argv[2];

    /* Initialise the stringv */
    assert(init_stringv(&s, block_size));

    /* Read the random string data, manicure it a bit, and then read it all
     * into the stringv */
    sbuf = read_strings(filename);
    terminate_at_newline(sbuf);
    fill_stringv(&s, sbuf);

    /* Do the profiling */
    result = profile(&s);
    report_profile_results(&s, sbuf, result);

    return 0;
}

clock_t profile(struct stringv *s)
{
    char const *it = NULL;
    clock_t start, diff;

    start = clock();
    for (it = stringv_begin(s);
            it != stringv_end(s);
            it = stringv_next(s, it)) {
        fprintf(stderr, "%s\n", it);
    }
    diff = clock() - start;

    return diff;
}

