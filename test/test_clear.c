#include <assert.h>
#include <string.h>

#include "test.h"
#include "../stringv.h"

static int test_clear_consistent(void);
static int test_clear_zero_buf(void);

static const test_case tests[] = {
    TEST_CASE(test_clear_consistent),
    TEST_CASE(test_clear_zero_buf)
};

int main(void)
{
    return !run_many(tests, TEST_COUNT(tests));
}

/* Checks that clearing a stringv leaves it in a state consistent with
 * stringv_clear's intended semantics: the mutable counts should be reset to
 * 0 and other fields should not be changed. */
int test_clear_consistent(void)
{
    struct stringv sv = STRINGV_ZERO;
    char buf[12], *p;
    int block_total, block_size;

    /* Init the stringv */
    assert(stringv_init(&sv, buf, 12, 3));

    /* These fields should not change after a call to stringv_clear */
    p = sv.buf;
    block_total = sv.block_total;
    block_size = sv.block_size;

    /* Modify the counts so we have something to compare against */
    ++sv.block_used;
    ++sv.string_count;

    assert(sv.block_used != 0);
    assert(sv.string_count != 0);

    stringv_clear(&sv);

    return sv.buf == p
        && sv.block_total == block_total
        && sv.block_size == block_size
        && sv.block_used == 0
        && sv.string_count == 0;
}

/* Checks that stringv_clear correctly zeroes the stringv buffer */
int test_clear_zero_buf(void)
{
    struct stringv sv = STRINGV_ZERO;
    char buf[12];
    char data[12] = {'a','a',0,'b','b',0,'c','c',0,'d','d',0};
    int i;

    /* Init the stringv, then write to its buffer to simulate usage */
    assert(stringv_init(&sv, buf, 12, 3));
    memcpy(sv.buf, data, 12);

    stringv_clear(&sv);

    /* Now check that the buffer is zeroed */
    for (i = 0; i < 12; ++i) {
        if (sv.buf[i]) {
            return 0;
        }
    }

    return 1;
}

