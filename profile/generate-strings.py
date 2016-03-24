#!/usr/bin/env python3
import numpy
import string
import random
import itertools
import argparse
import sys

# The characters we sample from are the printable characters.
CHARS = string.ascii_lowercase + string.ascii_uppercase + string.digits

def random_strings(f):
    """
    Generates random strings (with chars taken from CHARS) with lengths that
    are determined at each iteration by calling the function f. f should take
    no arguments and return an int.
    """
    while True:
        yield ''.join(random.choice(CHARS) for _ in range(f()))

def main():
    p = argparse.ArgumentParser()
    p.add_argument('num', type=int, help='The number of strings to generate.')
    sp = p.add_subparsers(dest='command')

    # Identical length subparser
    sp_identical = sp.add_parser('identical', help='Generate strings with '
            'identical lengths (no variability)')
    sp_identical.add_argument('length', type=int, help='Length of strings')

    # Uniform distribution subparser
    sp_uniform = sp.add_parser('uniform', help='Generate strings with lengths '
            'distributed according to the discrete uniform distribution')
    sp_uniform.add_argument('a', type=int, help='Lower bound')
    sp_uniform.add_argument('b', type=int, help='Upper bound (inclusive)')

    # Binomial distribution subparser
    sp_binomial = sp.add_parser('binomial', help='Generate strings with '
            'lengths distributed according to the binomial distribution')
    sp_binomial.add_argument('n', type=int, help='Number of trials')
    sp_binomial.add_argument('p', type=float, help='Success probability')
    args = p.parse_args()

    # Check the args and determine the required generator
    dist = None
    if args.command == 'identical':
        assert args.length > 0
        dist = lambda: args.length
    elif args.command == 'uniform':
        assert args.a > 0 and args.b > 0 and args.b > args.a
        dist = lambda: numpy.random.random_integers(args.a, args.b)
    elif args.command == 'binomial':
        assert args.n > 0 and 0 < args.p < 1
        dist = lambda: numpy.random.binomial(args.n, args.p)
    else:
        sys.exit(1)

    # Dump the required number of strings to stdout
    for i in itertools.islice(random_strings(dist), args.num):
        print(i)

    sys.exit(0)

if __name__ == '__main__':
    main()
