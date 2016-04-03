#include <assert.h>
#include <string.h>

#include "test.h"
#include "../stringv.h"

static int test_sort_bad_params(void);
static int test_sort_trivial_0(void);
static int test_sort_trivial_1(void);
static int test_sort_correct(void);

static const test_case tests[] = {
    TEST_CASE(test_sort_bad_params),
    TEST_CASE(test_sort_trivial_0),
    TEST_CASE(test_sort_trivial_1),
    TEST_CASE(test_sort_correct)
};

int main(void)
{
    return !run_many(tests, TEST_COUNT(tests));
}

int test_sort_bad_params(void)
{
    struct stringv s = STRINGV_ZERO;
    char buf[24] = {0};

    assert(stringv_init(&s, buf, 24, 8));

    return !stringv_sort(NULL, strncmp)
        && !stringv_sort(&s, NULL);
}

int test_sort_trivial_0(void)
{
    struct stringv s = STRINGV_ZERO;
    char buf[24] = {0};

    assert(stringv_init(&s, buf, 24, 8));

    return !!stringv_sort(&s, strncmp);
}

int test_sort_trivial_1(void)
{
    struct stringv s = STRINGV_ZERO;
    char buf[24] = {0};

    assert(stringv_init(&s, buf, 24, 8));
    assert(stringv_push_back(&s, "AAAAAAA", 7));

    return !!stringv_sort(&s, strncmp);
}

int test_sort_correct(void)
{
    struct stringv s = STRINGV_ZERO;
    char buf[24] = {0};
    static char const *expected[] = {"AAAAAAA", "BBBBBBB", "CCCCCCC"};
    int i = 0;

    assert(stringv_init(&s, buf, 24, 8));
    assert(stringv_push_back(&s, "CCCCCCC", 7));
    assert(stringv_push_back(&s, "BBBBBBB", 7));
    assert(stringv_push_back(&s, "AAAAAAA", 7));

    if (!stringv_sort(&s, strncmp)) {
        return 0;
    }

    for (i = 0; i < s.string_count; ++i) {
        if (strcmp(stringv_get(&s, i), expected[i]) != 0) {
            return 0;
        }
    }

    return 1;
}
