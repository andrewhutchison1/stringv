#include <stdio.h>
#include <assert.h>

#include "bench.h"
#include "../stringv.h"

/* Makes a fixed-size stringv */
static struct stringv *allocate(struct stringv *s);

int main(int argc, char **argv)
{
}

struct stringv *allocate(struct stringv *s)
{
    char *data = calloc(ALLOCATION_SIZE, 1);
    assert(data);
    assert(s);

    return stringv_init(s, data, 
}
