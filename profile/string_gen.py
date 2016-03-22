#!/usr/bin/env python3
import numpy as np
import string
import random
import sys
import itertools

# The characters we sample from are the printable characters.
CHARS = string.ascii_lowercase + string.ascii_uppercase + string.digits

# Returns a random string of the given size. We don't care about the
# distribution here so just use python's (supposedly uniform) sampler.
def string_gen(size):
    return ''.join(random.choice(CHARS) for _ in range(size))

# Returns a generator that yields random strings uniformly distributed in
# the given range.
def uniform_strings(low, high):
    while True:
        yield string_gen(np.random.random_integers(low, high))

# Returns a generator that yields random strings distributed according to the
# binomial distribution. The binomial distribution behaves like the normal
# distribution for large n so this can be used to approximate normally
# distributed string lengths.
def binomial_strings(n, p):
    while True:
        yield string_gen(np.random.binomial(n, p))

def main(argv):
    assert len(argv) == 5
    dist,num= argv[1:3]
    n = int(num)

    if n <= 0:
        print("Invalid count argument", file=sys.stderr)
        sys.exit(1)

    gen = None
    if dist == 'uniform':
        gen = uniform_strings(int(argv[3]), int(argv[4]))
    elif dist == 'binomial':
        gen = binomial_strings(int(argv[3]), float(argv[4]))
    else:
        print("Invalid distribution argument!", file=sys.stderr)
        sys.exit(1)

    for string in itertools.islice(gen, 0, n):
        print(string)

    sys.exit(0)

if __name__ == '__main__':
    main(sys.argv)
