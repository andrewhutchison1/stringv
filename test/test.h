#ifndef STRINGV_TEST_H_
#define STRINGV_TEST_H_

#include <assert.h>

/* DRY for forming arrays of the above type */
#define NAMED_TEST(name) {name,#name}

/* Get the number of named_tests in an array */
#define NAMED_TEST_COUNT(named_tests) \
    (sizeof((named_tests))/sizeof(named_test))

/* Define this to 0 (via makefile or otherwise) to hide the output of
 * succeeded tests */
#ifndef TEST_SHOW_SUCCESS
#define TEST_SHOW_SUCCESS 1
#endif

/* Some tests rely on functions that should not (but may) fail. In the case
 * that they fail, the test itself didn't actually fail since the function
 * under review didn't get a chance to fail. Instead we assert its success
 * to give a hard error to signal an out-of-band error. */
#undef NDEBUG
#define ASSERT_SUCCESS(expr) assert((expr) && "This should have succeeded")

/* Test functions are stateless, return 1 on success and 0 on failure */
typedef int (*test_function)(void);

/* Tests are bundled together with their name for IO */
typedef struct {
    test_function function;
    char const *const name;
} named_test;

static void run_tests(
        char const *name,
        named_test const *named_tests,
        size_t n)
{
    size_t i, failed = 0;

    ASSERT_SUCCESS(name && named_tests && n);
    puts(name);

    for (i = 0; i < n; ++i) {
        if (!named_tests[i].function()) {
            printf("\t%s failed.\n", named_tests[i].name);
            ++failed;
        } else {
#if TEST_SHOW_SUCCESS == 1
            printf("\t%s succeeded.\n", named_tests[i].name);
#endif
        }
    }

    printf("\t*** Summary: ran %u, failed %u\n",
            (unsigned)n,
            (unsigned)failed);
}

#endif /* STRINGV_TEST_H_ */

