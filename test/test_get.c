#include <assert.h>
#include <string.h>

#include "test.h"
#include "../stringv.h"

static int test_get_params_bad(void);
static int test_get_params_good(void);
static int test_get_unchanged(void);

static const test_case tests[] = {
    TEST_CASE(test_get_params_bad),
    TEST_CASE(test_get_params_good),
    TEST_CASE(test_get_unchanged)
};

int main(void)
{
    return !run_many(tests, TEST_COUNT(tests));
}

int test_get_params_bad(void)
{
    struct stringv stringv = STRINGV_ZERO;
    char buf[12] = {0};

    assert(stringv_init(&stringv, buf, 12, 4));
    assert(stringv_push_back(&stringv, "Hello", 5));

    return !stringv_get(NULL, 0)
        && !stringv_get(&stringv, -1)
        && !stringv_get(&stringv, stringv.string_count);
}

int test_get_params_good(void)
{
    struct stringv stringv = STRINGV_ZERO;
    char buf[12] = {0};

    assert(stringv_init(&stringv, buf, 12, 4));
    assert(stringv_push_back(&stringv, "Hello", 5));

    return stringv_get(&stringv, 0)
        && strcmp(stringv_get(&stringv, 0), "Hello") == 0;
}

int test_get_unchanged(void)
{
    struct stringv stringv = STRINGV_ZERO;
    char buf[20] = {0};

    assert(stringv_init(&stringv, buf, 20, 10));
    assert(stringv_push_back(&stringv, "String", 6));
    assert(stringv_get(&stringv, 0));

    return stringv.block_size == 10
        && stringv.block_used == 1
        && stringv.string_count == 1
        && stringv.block_total == 2;
}
