#include <assert.h>
#include <string.h>

#include "test.h"
#include "../stringv.h"

static int test_insert_params_bad(void);
static int test_insert_params_good(void);
static int test_insert_many(void);

static const test_case tests[] = {
    TEST_CASE(test_insert_params_bad),
    TEST_CASE(test_insert_params_good),
    TEST_CASE(test_insert_many)
};

int main(void)
{
    return !run_many(tests, TEST_COUNT(tests));
}

int test_insert_params_bad(void)
{
    struct stringv stringv = STRINGV_ZERO;
    char buf[15] = {0};

    assert(stringv_init(&stringv, buf, 15, 5));

    return !stringv_insert(NULL, "Hello", 5, 0)
        && !stringv_insert(&stringv, NULL, 5, 0)
        && !stringv_insert(&stringv, "Hello", 0, 0)
        && !stringv_insert(&stringv, "Hello", -1, 0)
        && !stringv_insert(&stringv, "Hello", 234, 0)
        && !stringv_insert(&stringv, "Hello", 5, 1);
}

int test_insert_params_good(void)
{
    struct stringv stringv = STRINGV_ZERO;
    char buf[15] = {0};

    assert(stringv_init(&stringv, buf, 15, 5));

    return !!stringv_insert(&stringv, "Hello", 5, 0)
        && strcmp(stringv_get(&stringv, 0), "Hello") == 0;
}

int test_insert_many(void)
{
    struct stringv stringv = STRINGV_ZERO;
    char buf[15] = {0};

    assert(stringv_init(&stringv, buf, 15, 5));

    stringv_insert(&stringv, "AAAA", 4, 0);
    stringv_insert(&stringv, "CCCC", 4, 1);
    stringv_insert(&stringv, "BBBB", 4, 1);

    return stringv.string_count == 3
        && stringv.block_used == 3
        && strcmp(stringv_get(&stringv, 0), "AAAA") == 0
        && strcmp(stringv_get(&stringv, 1), "BBBB") == 0
        && strcmp(stringv_get(&stringv, 2), "CCCC") == 0;
}
