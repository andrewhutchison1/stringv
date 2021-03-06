#ifndef TEST_H_
#define TEST_H_

#include <stddef.h>

#include "../stringv.h"

/* Test functions must not take any arguments. They return 0 on failure and
 * a nonzero value on success */
typedef int (*test_function)(void);

/* Test cases simply associate a test function with a string name for IO */
typedef struct {
    test_function function;
    char const *const name;
} test_case;

/* DRY macro to define a test case. */
#define TEST_CASE(name) {name,#name}

/* Number of items in test case array */
#define TEST_COUNT(tests) (sizeof((tests))/sizeof((tests)[0]))

/* Runs a single test. Returns a value indicating test failure or success
 * suitable for returning from main */
int run_test(test_case const* test);

/* Runs each test in the given array of test cases. Returns 0 if all tests
 * succeed, and zero otherwise */
int run_many(test_case const *tests, int n);

/* Dumps a block of raw memory to stdout */
void dump_memory(char const *ptr, int size, int groupby);

/* Dumps a stringv's buffer to stdout */
void dump_stringv(struct stringv const *stringv);

#endif /* TEST_H_ */
