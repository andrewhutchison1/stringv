#include "profile.h"
#include "../stringv.h"

char *buf = NULL;

static double random_access_stringv(
        struct profile_sample const *sample,
        struct stringv const *stringv,
        int replicates);

static double random_access_naive(
        struct profile_sample const *sample,
        int replicates);

int profile_init(struct profile_sample const *sample)
{
    assert(sample);
    (void)sample;

    buf = calloc(1, STRINGV_ALLOCATION);
    if (!buf) {
        return 0;
    }

    puts("Block size,stringv,naive");
    return 1;
}

void profile_function(
        struct profile_sample const *sample,
        int block_size,
        int replicates)
{
    struct stringv s = STRINGV_ZERO;

    assert(sample);

    /* Clear the global buffer */
    assert(buf);
    memset(buf, 0, STRINGV_ALLOCATION);

    /* Initialise the global buffer with the given block size */
    stringv_init(&s, buf, STRINGV_ALLOCATION, block_size);

    /* Fill the stringv with the sample strings (they are newline delimited) */
    stringv_split(&s, sample->buf, (int)sample->size, '\n');

    /* Run the profiles */
    printf("%d,%f,%f\n", block_size,
            random_access_stringv(sample, &s, replicates),
            random_access_naive(sample, replicates));
}

double random_access_stringv(
        struct profile_sample const *sample,
        struct stringv const *stringv,
        int replicates)
{
    volatile char const *P = NULL;
    int i = 0, j = 0, r = 0;
    clock_t start = 0;
    double sum = 0.;

    srand(RAND_SEED);

    for (i = 0; i < replicates; ++i) {
        r = rand_range(0, sample->count);

        start = clock();
        P = stringv_get(stringv, r);
        sum += (clock() - start);
    }

    return sum / replicates;
}

double random_access_naive(
        struct profile_sample const *sample,
        int replicates)
{
    volatile char const *P = NULL;
    int i = 0, j = 0, r = 0;
    clock_t start = 0;
    double sum = 0.;

    srand(RAND_SEED);

    for (i = 0; i < replicates; ++i) {
        r = rand_range(0, sample->count);

        start = clock();
        while (r > 0) {
            if (!sample->buf[j]) {
                --r;
            }

            ++j;
        }
        P = sample->buf + j;
        sum += (clock() - start);
        j = 0;
    }

    return sum / replicates;
}
