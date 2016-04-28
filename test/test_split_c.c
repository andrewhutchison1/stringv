#include <assert.h>
#include <string.h>
#include <stdio.h>

#include "test.h"
#include "../stringv.h"

static int test_split_c_params_bad(void);
static int test_split_c_correct(void);
static int test_split_c_partial(void);
static int test_split_c_sep_first(void);
static int test_split_c_sep_last(void);
static int test_split_c_sep_repeat(void);

static const test_case tests[] = {
    TEST_CASE(test_split_c_params_bad),
    TEST_CASE(test_split_c_correct),
    TEST_CASE(test_split_c_partial),
    TEST_CASE(test_split_c_sep_first),
    TEST_CASE(test_split_c_sep_last),
    TEST_CASE(test_split_c_sep_repeat)
};

int main(void)
{
    return !run_many(tests, TEST_COUNT(tests));
}

int test_split_c_params_bad(void)
{
    struct stringv s = STRINGV_ZERO;
    char buf[24] = {0};
    char const *const string = "abc,def,ghi";

    assert(stringv_init(&s, buf, 24, 8));

    return !stringv_split_c(NULL, string, (int)strlen(string) + 1, ',')
        && !stringv_split_c(&s, NULL, (int)strlen(string) + 1, ',')
        && !stringv_split_c(&s, string, 0, ',');
}

int test_split_c_correct(void)
{
    struct stringv s = STRINGV_ZERO;
    char buf[24] = {0};
    char const *const string = "abc,def,ghi";
    char const *const expected[] = {"abc", "def", "ghi"};
    int result = 0, i = 0;

    assert(stringv_init(&s, buf, 24, 8));

    result = stringv_split_c(&s, string, (int)strlen(string) + 1, ',');
    if (result != (int)strlen(string) + 1) {
        return 0;
    }

    if (s.string_count != 3) {
        return 0;
    }

    for (i = 0; i < 3; ++i) {
        if (strcmp(stringv_get(&s, i), expected[i]) != 0) {
            return 0;
        }
    }

    return 1;
}

int test_split_c_partial(void)
{
    struct stringv s = STRINGV_ZERO;
    char buf[12] = {0};
    char const *const string = "abcdef,ghijkl";
    char const *const expected = "abcdef";
    int result = 0;

    assert(stringv_init(&s, buf, 12, 6));

    result = stringv_split_c(&s, string, (int)strlen(string) + 1, ',');
    if (result != 7 || s.string_count != 1) {
        return 0;
    }

    if (strcmp(stringv_get(&s, 0), expected) != 0) {
        return 0;
    }

    return 1;
}

int test_split_c_sep_first(void)
{
    struct stringv s = STRINGV_ZERO;
    char buf[12] = {0};
    char const *const string = ",abc,def";
    char const *const expected[] = {"abc", "def"};
    int result = 0, i = 0;

    assert(stringv_init(&s, buf, 24, 8));

    result = stringv_split_c(&s, string, (int)strlen(string), ',');
    if (result != (int)strlen(string)) {
        return 0;
    }

    if (s.string_count != 2) {
        return 0;
    }

    for (i = 0; i < 2; ++i) {
        if (strcmp(stringv_get(&s, i), expected[i]) != 0) {
            return 0;
        }
    }

    return 1;
}

int test_split_c_sep_last(void)
{
    return 0;
}

int test_split_c_sep_repeat(void)
{
    return 0;
}

