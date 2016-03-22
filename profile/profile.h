#ifndef BENCH_H_
#define BENCH_H_

#include <stdio.h>

#define ALLOCATION_SIZE (4*1024)

/* Reads strings from stdin into a malloc'd buffer */
char const *read_strings(void);

#endif /* BENCH_H_ */
