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
static int test_copy_partial(void);

static const test_case tests[] = {
    TEST_CASE(test_copy_params_bad),
    TEST_CASE(test_copy_source_unchanged),
    TEST_CASE(test_copy_block_size_unchanged),
    TEST_CASE(test_copy_count),
    TEST_CASE(test_copy_succeeds_one_to_one),
    TEST_CASE(test_copy_succeeds_overfull),
    TEST_CASE(test_copy_succeeds_underfull),
    TEST_CASE(test_copy_partial)
};

int main(void)
{
    return !run_many(tests, TEST_COUNT(tests));
}

/* Tests that bad parameters are reported correctly */
int test_copy_params_bad(void)
{
    struct stringv s1 = STRINGV_ZERO;
    struct stringv s2 = STRINGV_ZERO;
    char b1[20] = {0};
    char b2[20] = {0};

    assert(stringv_init(&s1, b1, 20, 10));
    assert(stringv_init(&s2, b2, 20, 10));

    return !stringv_copy(NULL, &s2)
        && !stringv_copy(&s1, NULL);
}

/* Tests that the source stringv is not modified by stringv_copy */
int test_copy_source_unchanged(void)
{
    struct stringv s1 = STRINGV_ZERO, s2 = STRINGV_ZERO;
    char b1[9], b2[9];

    assert(stringv_init(&s1, b1, 9, 3));
    assert(stringv_init(&s2, b2, 9, 3));

    memcpy(s1.buf, "ab\0cd\0ef", 9);
    s1.block_used = s1.string_count = 3;

    return stringv_copy(&s2, &s1) == 3
        && memcmp(s1.buf, "ab\0cd\0ef", 9) == 0;
}

/* Ensures that stringv_copy does not touch the block size parameter of the
 * destination stringv */
int test_copy_block_size_unchanged(void)
{
    struct stringv s1 = STRINGV_ZERO, s2 = STRINGV_ZERO;
    char b1[9], b2[12];

    assert(stringv_init(&s1, b1, 9, 3));
    assert(stringv_init(&s2, b2, 12, 4));

    memcpy(s1.buf, "ab\0cd\0ef", 9);
    s1.block_used = s1.string_count = 3;

    return stringv_copy(&s2, &s1) == 3
        && s2.block_size == 4;
}

/* Tests that the string count is preserved after a call to stringv_copy */
int test_copy_count(void)
{
    struct stringv s1 = STRINGV_ZERO, s2 = STRINGV_ZERO;
    char b1[9], b2[12];

    assert(stringv_init(&s1, b1, 9, 3));
    assert(stringv_init(&s2, b2, 12, 4));

    memcpy(s1.buf, "ab", 3);
    s1.block_used = s1.string_count = 1;

    return stringv_copy(&s2, &s1) == 1
        && s2.string_count == 1;
}

/* Tests successful copying for a one-to-one copy operation. This hits
 * the most optimal case of stringv_copy, where the source and destination
 * stringvs have the same dimensions. */
int test_copy_succeeds_one_to_one(void)
{
    struct stringv s1 = STRINGV_ZERO, s2 = STRINGV_ZERO;
    char b1[9], b2[9];

    assert(stringv_init(&s1, b1, 9, 3));
    assert(stringv_init(&s2, b2, 9, 3));

    memcpy(s1.buf, "ab\0cd\0ef", 9);
    s1.block_used = s1.string_count = 3;

    return stringv_copy(&s2, &s1) == 3
        && memcmp(s2.buf, "ab\0cd\0ef", 9) == 0
        && s2.block_used == 3
        && s2.string_count == 3;
}

/* Tests successful copying for a copy from an overfull stringv. This hits
 * the most general code path where the number of required blocks must be
 * recomputed for each string */
int test_copy_succeeds_overfull(void)
{
    struct stringv s1 = STRINGV_ZERO, s2 = STRINGV_ZERO;
    char b1[6], b2[12];

    assert(stringv_init(&s1, b1, 6, 3));
    assert(stringv_init(&s2, b2, 12, 6));

    memcpy(s1.buf, "abcde", 6);
    s1.block_used = 2;
    s1.string_count = 1;

    return stringv_copy(&s2, &s1) == 1
        && s2.block_used == 1
        && s2.string_count == 1;
}

/* Tests that an underfull stringv (one where at least one entire block is
 * zero) copies correctly */
int test_copy_succeeds_underfull(void)
{
    struct stringv s1 = STRINGV_ZERO, s2 = STRINGV_ZERO;
    char b1[12], b2[8];

    assert(stringv_init(&s1, b1, 12, 3));
    assert(stringv_init(&s2, b2, 8, 4));

    memcpy(s1.buf, "abc\0\0\0def\0\0", 12);
    s1.block_used = 4;
    s1.string_count = 2;

    return stringv_copy(&s2, &s1) == 2
        && memcmp(s2.buf, "abc\0def", 8) == 0
        && s2.block_used == 2
        && s2.string_count == 2;
}

int test_copy_partial(void)
{
    struct stringv s1 = STRINGV_ZERO, s2 = STRINGV_ZERO;
    char b1[6], b2[4];

    assert(stringv_init(&s1, b1, 6, 3));
    assert(stringv_init(&s2, b2, 4, 2));

    memcpy(s1.buf, "ab\0cd", 6);
    s1.block_used = 2;
    s1.string_count = 1;

    /* Only the first string should be copied */
    return stringv_copy(&s2, &s1) == 1
        && s2.block_used == 2
        && s2.string_count == 1;
}
