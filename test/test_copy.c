#include <assert.h>
#include <string.h>

#include "test.h"
#include "../stringv.h"

static int test_copy_params_bad(void);
static int test_copy_source_unchanged(void);
static int test_copy_block_size_unchanged(void);
static int test_copy_count(void);
static int test_copy_succeeds_one_to_one(void);
static int test_copy_succeeds_overfull(void);
static int test_copy_succeeds_underfull(void);

static const test_case tests[] = {
    TEST_CASE(test_copy_params_bad),
    TEST_CASE(test_copy_source_unchanged),
    TEST_CASE(test_copy_block_size_unchanged),
    TEST_CASE(test_copy_count),
    TEST_CASE(test_copy_succeeds_one_to_one),
    TEST_CASE(test_copy_succeeds_overfull),
    TEST_CASE(test_copy_succeeds_underfull)
};

int main(void)
{
    return !run_many(tests, TEST_COUNT(tests));
}

int test_copy_params_bad(void)
{
    struct stringv sv1 = STRINGV_ZERO;
    struct stringv sv2 = STRINGV_ZERO;
    char buf1[20] = {0};
    char buf2[20] = {0};

    assert(stringv_init(&sv1, buf1, 20, 10));
    assert(stringv_init(&sv2, buf2, 20, 10));

    return !stringv_copy(NULL, &sv2)
        && !stringv_copy(&sv1, NULL);
}

int test_copy_source_unchanged(void)
{
    struct stringv sv1 = STRINGV_ZERO;
    struct stringv sv2 = STRINGV_ZERO;
    char buf1[9] = {'a','b','\0','c','d','\0','e','f','\0'};
    char save[9];
    char buf2[9] = {0};

    memcpy(save, buf1, 9);

    assert(stringv_init(&sv1, buf1, 9, 3));
    assert(stringv_init(&sv2, buf2, 9, 3));

    memcpy(sv1.buf, save, 9);
    assert(stringv_copy(&sv2, &sv1));

    return memcmp(sv1.buf, save, 9) == 0;
}

int test_copy_block_size_unchanged(void)
{
    struct stringv sv1 = STRINGV_ZERO;
    struct stringv sv2 = STRINGV_ZERO;
    char buf1[9] = {0};
    char buf2[10] = {0};

    assert(stringv_init(&sv1, buf1, 9, 3));
    assert(stringv_init(&sv2, buf2, 10, 4));
    assert(stringv_copy(&sv2, &sv1));

    return sv2.block_size == 4;
}

int test_copy_count(void)
{
    struct stringv sv1 = STRINGV_ZERO;
    struct stringv sv2 = STRINGV_ZERO;
    char buf1[9] = {0};
    char buf2[10] = {0};

    assert(stringv_init(&sv1, buf1, 9, 3));
    assert(stringv_init(&sv2, buf2, 10, 4));

    memcpy(sv1.buf, "ab", 3);
    sv1.string_count = 1;
    sv1.block_used = 1;
    assert(stringv_copy(&sv2, &sv1));

    return sv2.string_count = 1;
}

int test_copy_succeeds_one_to_one(void)
{
    struct stringv sv1, sv2;
    char b1[9], b2[9];
    char save[9] = {'a','b',0,'c','d',0,'e','f',0};

    assert(stringv_init(&sv1, b1, 9, 3));
    assert(stringv_init(&sv2, b2, 9, 3));

    memcpy(sv1.buf, save, 9);
    sv1.block_used = 3;
    sv1.string_count = 3;

    assert(stringv_copy(&sv2, &sv1));

    return memcmp(sv2.buf, save, 9) == 0
        && sv2.block_used == 3
        && sv2.string_count == 3;
}

int test_copy_succeeds_overfull(void)
{
    struct stringv sv1, sv2;
    char b1[6], b2[12];
    char save[6] = {'a','b','c','d','e',0};

    assert(stringv_init(&sv1, b1, 6, 3));
    assert(stringv_init(&sv2, b2, 12, 6));

    memcpy(sv1.buf, save, 5);
    sv1.block_used = 2;
    sv1.string_count = 1;

    assert(stringv_copy(&sv2, &sv1));

    return memcmp(sv2.buf, save, 6) == 0
        && sv2.block_used == 1
        && sv2.string_count == 1;
}

int test_copy_succeeds_underfull(void)
{
    struct stringv sv1, sv2;
    char b1[12], b2[8];
    char save[] = {'a','b','c',0,0,0,'d','e','f',0,0,0};
    char save1[] = {'a','b','c',0,'d','e','f',0};

    assert(stringv_init(&sv1, b1, 12, 3));
    assert(stringv_init(&sv2, b2, 8, 4));

    /* Mock out the insertion of a few strings */
    memcpy(sv1.buf, save, 12);
    sv1.block_used = 4;
    sv1.string_count = 2;

    assert(stringv_copy(&sv2, &sv1));

    return memcmp(sv2.buf, save1, 8) == 0
        && sv2.block_used == 2
        && sv2.string_count == 2;
}
