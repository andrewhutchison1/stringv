#include <assert.h>
#include <string.h>

#include "test.h"

static int test_push_back_params_bad(void);
static int test_push_back_params_good(void);
static int test_push_back_success_state(void);
static int test_push_back_failure_space(void);

static const test_case tests[] = {
    TEST_CASE(test_push_back_params_bad),
    TEST_CASE(test_push_back_params_good),
    TEST_CASE(test_push_back_success_state),
    TEST_CASE(test_push_back_failure_space)
};

int main(void)
{
    return !run_many(tests, TEST_COUNT(tests));
}

/* Tests that bad parameters are correctly reported */
int test_push_back_params_bad(void)
{
    struct stringv sv;
    char buf[12];

    assert(stringv_init(&sv, buf, 12, 4));

    return !stringv_push_back(NULL, "abc", 3)
        && !stringv_push_back(&sv, NULL, 3)
        && !stringv_push_back(&sv, "abc", 0)
        && !stringv_push_back(&sv, "abc", -12342);
}

/* Tests that good parameters succeed */
int test_push_back_params_good(void)
{
    struct stringv sv;
    char buf[16];
    int ret;

    assert(stringv_init(&sv, buf, 16, 4));

    ret = !!stringv_push_back(&sv, "abcde", 5);
    ret = ret && !!stringv_push_back(&sv, "abcd", 4);

    return ret;
}

/* Tests that a push back operation leaves the stringv in a consistent
 * state */
int test_push_back_success_state(void)
{
    struct stringv sv;
    char buf[12];
    char const *ptr;

    assert(stringv_init(&sv, buf, 12, 4));

    ptr = stringv_push_back(&sv, "abcd", 4);
    assert(ptr);

    return strcmp(ptr, "abcd") == 0
        && sv.block_used == 2
        && sv.string_count == 1;
}

/* Tests that a push back operation will correctly fail on a stringv that
 * has insufficient space to store the string */
int test_push_back_failure_space(void)
{
    struct stringv sv;
    char buf[18];

    assert(stringv_init(&sv, buf, 18, 6));          /* 3 blocks total */
    assert(stringv_push_back(&sv, "abcdef", 6));    /* Uses 2 blocks */

    return !stringv_push_back(&sv, "abcdef", 6)
        && sv.block_used == 2
        && sv.string_count == 1;
}
