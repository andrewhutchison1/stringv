#include <assert.h>
#include <string.h>

#include "test.h"
#include "../stringv.h"

static int test_iteration_string_equal_0(void);
static int test_iteration_string_equal_1(void);
static int test_iteration_get_equal_0(void);
static int test_iteration_get_equal_1(void);

static const test_case tests[] = {
    TEST_CASE(test_iteration_string_equal_0),
    TEST_CASE(test_iteration_string_equal_1),
    TEST_CASE(test_iteration_get_equal_0),
    TEST_CASE(test_iteration_get_equal_1)
};

int main(void)
{
    return !run_many(tests, TEST_COUNT(tests));
}

int test_iteration_string_equal_0(void)
{
    struct stringv s = STRINGV_ZERO;
    char b[15] = {0};
    char const *const strings[] = {
        "AAAA", "BBBB", "CCCC"
    };
    char const *it = NULL;
    int i = 0;

    assert(stringv_init(&s, b, 15, 5));
    assert(stringv_push_back(&s, "AAAA", 4));
    assert(stringv_push_back(&s, "BBBB", 4));
    assert(stringv_push_back(&s, "CCCC", 4));

    for (it = stringv_begin(&s);
            it != stringv_end(&s);
            ++i, it = stringv_next(&s, it)) {
        if (strcmp(strings[i], it) != 0) {
            return 0;
        }
    }

    return it == stringv_end(&s);
}

int test_iteration_string_equal_1(void)
{
    struct stringv s = STRINGV_ZERO;
    char b[16] = {0};
    char const *const strings[] = {
        "AAA", "BBBBBBB", "CCC"
    };
    char const *it = NULL;
    int i = 0;

    assert(stringv_init(&s, b, 16, 4));
    assert(stringv_push_back(&s, "AAA", 3));
    assert(stringv_push_back(&s, "BBBBBBB", 7));
    assert(stringv_push_back(&s, "CCC", 3));

    for (it = stringv_begin(&s);
            it != stringv_end(&s);
            ++i, it = stringv_next(&s, it)) {
        if (strcmp(strings[i], it) != 0) {
            return 0;
        }
    }

    return it == stringv_end(&s);
}

int test_iteration_get_equal_0(void)
{
    struct stringv s = STRINGV_ZERO;
    char b[16] = {0};
    char const *it = NULL;
    int i = 0;

    assert(stringv_init(&s, b, 16, 4));
    assert(stringv_push_back(&s, "AAA", 3));
    assert(stringv_push_back(&s, "BBB", 3));
    assert(stringv_push_back(&s, "CCC", 3));
    assert(stringv_push_back(&s, "DDD", 3));

    for (it = stringv_begin(&s);
            it != stringv_end(&s);
            ++i, it = stringv_next(&s, it)) {
        if (it != stringv_get(&s, i)) {
            return 0;
        }
    }

    return it == stringv_end(&s);
}

int test_iteration_get_equal_1(void)
{
    struct stringv s = STRINGV_ZERO;
    char b[16] = {0};
    char const *it = NULL;
    int i = 0;

    assert(stringv_init(&s, b, 16, 4));
    assert(stringv_push_back(&s, "AAA", 3));
    assert(stringv_push_back(&s, "BBBBBBB", 7));
    assert(stringv_push_back(&s, "CCC", 3));

    for (it = stringv_begin(&s);
            it != stringv_end(&s);
            ++i, it = stringv_next(&s, it)) {
        if (it != stringv_get(&s, i)) {
            return 0;
        }
    }

    return it == stringv_end(&s);
}
