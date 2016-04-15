#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include "profile.h"

extern int profile_init(struct profile_sample *const sample);

extern void profile_function(
        struct profile_sample *const sample,
        int block_size,
        int replicates);

/* Prints the usage to stdout. Takes as an argument the executable name. */
static void print_usage(char const *name);

int main(int argc, char **argv)
{
    struct profile_sample sample = {NULL, 0};
    char const *sample_file = NULL;
    int block_size_min = 0, block_size_max = 0, replicates = 0;
    int i = 0, j = 0;

    if (argc != 5) {
        print_usage(argv[0]);
        return 1;
    }

    sample_file = argv[1];
    block_size_min = atoi(argv[2]);
    block_size_max = atoi(argv[3]);
    replicates = atoi(argv[4]);

    if (block_size_min == 0 || block_size_max == 0 || replicates == 0) {
        print_usage(argv[0]);
        return 1;
    }

    assert(block_size_min < block_size_max);

    /* Generate the profile sample object from the sample file */
    if (!profile_sample_read(&sample, sample_file)) {
        printf("Failed to read sample file %s\n", sample_file);
        return 1;
    }

    /* Initialise the profile */
    if (!profile_init(&sample)) {
        puts("Failed to initialise profile");
        return 1;
    }

    for (i = block_size_min; i < block_size_max; ++i) {
        profile_function(&sample, i, replicates);
    }

    return 0;
}

void print_usage(char const *name)
{
    printf("Usage: %s <sample_file> <block_size_min> <block_size_max> "
            "<replicates>\n", name);
}
