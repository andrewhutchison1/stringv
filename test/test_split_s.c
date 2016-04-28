#include <assert.h>
#include <string.h>
#include <stdio.h>

#include "test.h"
#include "../stringv.h"

static int test_split_s_params_bad(void);
static int test_split_s_correct(void);
static int test_split_s_partial(void);
static int test_split_s_sep_first(void);
static int test_split_s_sep_last(void);
static int test_split_s_sep_repeat(void);

static const test_case tests[] = {
    TEST_CASE(test_split_s_params_bad),
    TEST_CASE(test_split_s_correct),
    TEST_CASE(test_split_s_partial),
    TEST_CASE(test_split_s_sep_first),
    TEST_CASE(test_split_s_sep_last),
    TEST_CASE(test_split_s_sep_repeat)
};

int main(void)
{
    return !run_many(tests, TEST_COUNT(tests));
}

int test_split_s_params_bad(void)
{
    struct stringv s = STRINGV_ZERO;
    char buf[24] = {0};
    char const *const string = "abcXXXdefXXXghi";
    char const *const sep = "XXX";
    int len = 0, seplen = 0;

    assert(stringv_init(&s, buf, 24, 8));

    len = (int)strlen(string);
    seplen = (int)strlen(sep);

    return !stringv_split_s(NULL, string, len, sep, seplen)
        && !stringv_split_s(&s, NULL, len, sep, seplen)
        && !stringv_split_s(&s, string, 0, sep, seplen)
        && !stringv_split_s(&s, string, len, NULL, seplen)
        && !stringv_split_s(&s, string, len, sep, 0);
}

int test_split_s_correct(void)
{
    struct stringv s = STRINGV_ZERO;
    char buf[24] = {0};
    char const *const string = "abcXXXdefXXXghi";
    char const *const expected[] = {"abc", "def", "ghi"};
    char const *const sep = "XXX";
    int result = 0, i = 0, seplen = 0;

    assert(stringv_init(&s, buf, 24, 8));
    seplen = (int)strlen(sep);

    result = stringv_split_s(&s, string, (int)strlen(string), sep, seplen);
    if (result != (int)strlen(string)) {
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

int test_split_s_partial(void)
{
    struct stringv s = STRINGV_ZERO;
    char buf[12] = {0};
    char const *const string = "abcdefXXXghijkl";
    char const *const expected = "abcdef";
    char const *const sep = "XXX";
    int result = 0, seplen = 0;

    assert(stringv_init(&s, buf, 12, 6));
    seplen = (int)strlen(sep);

    result = stringv_split_s(&s, string, (int)strlen(string), sep, seplen);
    if (result != 9 || s.string_count != 1) {
        return 0;
    }

    if (strcmp(stringv_get(&s, 0), expected) != 0) {
        return 0;
    }

    return 1;
}

int test_split_s_sep_first(void)
{
    struct stringv s = STRINGV_ZERO;
    char buf[24] = {0};
    char const *const string = "XXXabcXXXdef";
    char const *const expected[] = {"abc", "def"};
    char const *const sep = "XXX";
    int result = 0, i = 0, seplen = 0;

    assert(stringv_init(&s, buf, 24, 8));
    seplen = (int)strlen(sep);

    result = stringv_split_s(&s, string, (int)strlen(string), sep, seplen);
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

int test_split_s_sep_last(void)
{
    struct stringv s = STRINGV_ZERO;
    char buf[24] = {0};
    char const *const string = "abcXXXdefXXX";
    char const *const expected[] = {"abc", "def"};
    char const *const sep = "XXX";
    int result = 0, i = 0, seplen = 0;

    assert(stringv_init(&s, buf, 24, 8));
    seplen = (int)strlen(sep);

    result = stringv_split_s(&s, string, (int)strlen(string), sep, seplen);
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

int test_split_s_sep_repeat(void)
{
    struct stringv s = STRINGV_ZERO;
    char buf[24] = {0};
    char const *const string = "abcXXXXXXdef";
    char const *const expected[] = {"abc", "def"};
    char const *const sep = "XXX";
    int result = 0, i = 0, seplen = 0;

    assert(stringv_init(&s, buf, 24, 8));
    seplen = (int)strlen(sep);

    result = stringv_split_s(&s, string, (int)strlen(string), sep, seplen);
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

