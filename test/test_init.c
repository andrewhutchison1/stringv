#include <stdio.h>

#include "../stringv.h"
#include "test.h"

static int test_ptr_params(void);
static int test_range_params(void);
static int test_buf_zeroed(void);

static const named_test named_tests[] = {
    NAMED_TEST(test_ptr_params),
    NAMED_TEST(test_range_params),
    NAMED_TEST(test_buf_zeroed)
};

int main(void)
{
    run_tests("test_init", named_tests, NAMED_TEST_COUNT(named_tests));
    return 0;
}

/* Ensures that NULL pointer parameters to stringv_init are correctly
 * detected */
int test_ptr_params(void)
{
    char buf[10] = {0};
    struct stringv stringv = {NULL, 0, 0};

    if (stringv_init(NULL, buf, 10, 5) ||
            stringv_init(&stringv, NULL, 10, 5)) {
        return 0;
    }

    return 1;
}

/* Ensures that out-of-range size parameters are correctly detected */
int test_range_params(void)
{
    char buf[10] = {0};
    struct stringv stringv = {NULL, 0, 0};
    int result = 1;

    result = result
        && !stringv_init(&stringv, buf, 0, 5)
        && !stringv_init(&stringv, buf, 1, 5)
        && !stringv_init(&stringv, buf, 10, 0)
        && !stringv_init(&stringv, buf, 10, 1)
        && !stringv_init(&stringv, buf, 10, 11);

    return result;
}

/* Ensures that the buffer passed to stringv_init is correctly zeroed */
int test_buf_zeroed(void)
{
    char buf[5] = {'.', '.', '.', '.', '.'};
    struct stringv stringv = {NULL, 0, 0};
    int i;

    if (!stringv_init(&stringv, buf, 5, 2)) {
        return 0;
    }

    ASSERT_SUCCESS(stringv_init(&stringv, buf, 5, 2));

    for (i = 0; i < 5; ++i) {
        if (buf[i]) {
            return 0;
        }
    }

    return 1;
}
