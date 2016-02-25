#include "test.h"

#include <assert.h>
#include <stdio.h>

int run_test(test_case const* test)
{
    int result;
    assert(test);

    printf("%s:\t", test->name);
    result = test->function();
    printf("%s\n", result ? "succeeded" : "FAILED");
    return result;
}

int run_many(test_case const *tests, int n)
{
    int i, succeeded = 0;
    assert(tests);

    /* Run all the tests, counting the number that succeed */
    for (i = 0; i < n; ++i) {
        if (run_test(tests + i)) {
            ++succeeded;
        }
    }

    /* Print a summary */
    printf("*** Summary: %i/%i passed\n", succeeded, n);
    return succeeded == n;
}

void dump_memory(char const *ptr, int size, int groupby)
{
    int i;

    assert(ptr && size > 0);

    for (i = 0; i < size; ++i) {
        if (i && groupby && (i % groupby == 0)) {
            putchar(' ');
        }

        if (ptr[i]) {
            putchar(ptr[i]);
        } else {
            putchar('.');
        }
    }

    printf("\n");
}
