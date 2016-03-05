#include <assert.h>
#include <string.h>
#include <stdio.h>

#include "test.h"
#include "../stringv.h"

static int test_push_front_params_bad(void);
static int test_push_front_params_good(void);
static int test_push_front_many(void);

static const test_case tests[] = {
    TEST_CASE(test_push_front_params_bad),
    TEST_CASE(test_push_front_params_good),
    TEST_CASE(test_push_front_many)
};

int main(void)
{
    return !run_many(tests, TEST_COUNT(tests));
}

int test_push_front_params_bad(void)
{
    struct stringv stringv = STRINGV_ZERO;
    char buf[18] = {0};

    assert(stringv_init(&stringv, buf, 18, 6));

    return !stringv_push_front(NULL, "Hello", 5)
        && !stringv_push_front(&stringv, NULL, 5)
        && !stringv_push_front(&stringv, "Hello", 0)
        && !stringv_push_front(&stringv, "Hello", 18);
}

int test_push_front_params_good(void)
{
    struct stringv stringv = STRINGV_ZERO;
    char buf[20] = {0};

    assert(stringv_init(&stringv, buf, 20, 10));
    return !!stringv_push_front(&stringv, "Hello", 5);
}

int test_push_front_many(void)
{
    struct stringv stringv = STRINGV_ZERO;
    char buf[20] = {0};

    assert(stringv_init(&stringv, buf, 18, 6));
    assert(stringv_push_front(&stringv, "AAAAA", 5));
    assert(stringv_push_front(&stringv, "BBBBB", 5));
    assert(stringv_push_front(&stringv, "CCCCC", 5));

    return stringv.string_count == 3
        && stringv.block_used == 3
        && strcmp(stringv_get(&stringv, 0), "CCCCC") == 0
        && strcmp(stringv_get(&stringv, 1), "BBBBB") == 0
        && strcmp(stringv_get(&stringv, 2), "AAAAA") == 0;
}
