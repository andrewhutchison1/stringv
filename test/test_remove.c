#include <assert.h>
#include <string.h>
#include <stdio.h>

#include "test.h"
#include "../stringv.h"

static int test_remove_params_bad(void);
static int test_remove_params_good(void);
static int test_remove_empty(void);
static int test_remove_front(void);
static int test_remove_back(void);
static int test_remove_middle(void);

static const test_case tests[] = {
    TEST_CASE(test_remove_params_bad),
    TEST_CASE(test_remove_params_good),
    TEST_CASE(test_remove_empty),
    TEST_CASE(test_remove_front),
    TEST_CASE(test_remove_back),
    TEST_CASE(test_remove_middle)
};

int main(void)
{
    return !run_many(tests, TEST_COUNT(tests));
}

int test_remove_params_bad(void)
{
    struct stringv stringv = STRINGV_ZERO;
    char buf[12] = {0};

    assert(stringv_init(&stringv, buf, 12, 4));
    assert(stringv_push_back(&stringv, "AAA", 3));

    return !stringv_remove(NULL, 0)
        && !stringv_remove(&stringv, -1)
        && !stringv_remove(&stringv, 1);
}

int test_remove_params_good(void)
{
    struct stringv stringv = STRINGV_ZERO;
    char buf[12] = {0};

    assert(stringv_init(&stringv, buf, 12, 4));
    assert(stringv_push_back(&stringv, "AAA", 3));

    return stringv_remove(&stringv, 0)
        && stringv.string_count == 0
        && stringv.block_used == 0
        && stringv.buf[0] == '\0';
}

int test_remove_empty(void)
{
    struct stringv stringv = STRINGV_ZERO;
    char buf[12] = {0};

    assert(stringv_init(&stringv, buf, 12, 4));
    return !stringv_remove(&stringv, 0);
}

int test_remove_front(void)
{
    struct stringv stringv = STRINGV_ZERO;
    char buf[12] = {0};

    assert(stringv_init(&stringv, buf, 12, 4));
    assert(stringv_push_back(&stringv, "AAA", 3));
    assert(stringv_push_back(&stringv, "BBB", 3));
    assert(stringv_push_back(&stringv, "CCC", 3));

    return stringv_remove(&stringv, 0)
        && stringv.string_count == 2
        && stringv.block_used == 2
        && strcmp(stringv_get(&stringv, 0), "BBB") == 0
        && strcmp(stringv_get(&stringv, 1), "CCC") == 0;
}

int test_remove_back(void)
{
    struct stringv stringv = STRINGV_ZERO;
    char buf[12] = {0};

    assert(stringv_init(&stringv, buf, 12, 4));
    assert(stringv_push_back(&stringv, "AAA", 3));
    assert(stringv_push_back(&stringv, "BBB", 3));
    assert(stringv_push_back(&stringv, "CCC", 3));

    return stringv_remove(&stringv, 2)
        && stringv.string_count == 2
        && stringv.block_used == 2
        && strcmp(stringv_get(&stringv, 0), "AAA") == 0
        && strcmp(stringv_get(&stringv, 1), "BBB") == 0;
}

int test_remove_middle(void)
{
    struct stringv stringv = STRINGV_ZERO;
    char buf[12] = {0};

    assert(stringv_init(&stringv, buf, 12, 4));
    assert(stringv_push_back(&stringv, "AAA", 3));
    assert(stringv_push_back(&stringv, "BBB", 3));
    assert(stringv_push_back(&stringv, "CCC", 3));

    return stringv_remove(&stringv, 1)
        && stringv.string_count == 2
        && stringv.block_used == 2
        && strcmp(stringv_get(&stringv, 0), "AAA") == 0
        && strcmp(stringv_get(&stringv, 1), "CCC") == 0;
}
