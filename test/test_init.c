#include <assert.h>

#include "test.h"
#include "../stringv.h"

static int test_init_params_bad(void);
static int test_init_params_good(void);
static int test_init_zero_buf(void);

static const test_case tests[] = {
    TEST_CASE(test_init_params_bad),
    TEST_CASE(test_init_params_good),
    TEST_CASE(test_init_zero_buf)
};

int main(void)
{
    return !run_many(tests, TEST_COUNT(tests));
}

/* Tests that bad inputs to stringv_init correctly cause it to fail */
static int test_init_params_bad(void)
{
    struct stringv sv = STRINGV_ZERO;
    char buf[10];

    return !stringv_init(NULL, buf, 10, 5)
        && !stringv_init(&sv, NULL, 10, 5)
        && !stringv_init(&sv, buf, 1, 5)
        && !stringv_init(&sv, buf, 10, 1)
        && !stringv_init(&sv, buf, 10, 11);
}

/* Tests that good inputs to stringv_init succeed correctly */
static int test_init_params_good(void)
{
    struct stringv sv = STRINGV_ZERO;
    char buf[10];
    return !!stringv_init(&sv, buf, 10, 5);
}

/* Tests that stringv_init correctly zeroes the buffer, which is an extremely
 * important precondition of most stringv operations */
static int test_init_zero_buf(void)
{
    struct stringv sv = STRINGV_ZERO;
    char buf[10];
    int i;

    assert(stringv_init(&sv, buf, 10, 5));

    for (i = 0; i < 10; ++i) {
        if (sv.buf[i]) {
            return 0;
        }
    }

    return 1;
}
