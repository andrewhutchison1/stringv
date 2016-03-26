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

def preamble(n, distribution, params):
    """
    Returns information about the string sample being generated as a string.
    This information includes the number of strings, and some basic
    statistical properties about the string lengths.
    """
    mean = 0
    variance = 0
    if distribution == 'identical':
        mean,variance = params[0],0
    elif distribution == 'uniform':
        mean = sum(params) / 2
        variance = ((params[1] - params[0] + 1) ** 2) / 12
    elif distribution == 'binomial':
        mean = params[0]*params[1]
        variance = mean * (1 - params[1])

    return '# {0} {1} {2}'.format(n, mean, variance)

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

    dist = None
    params = None
    assert args.num > 0

    if args.command == 'identical':
        assert args.length > 0
        dist = lambda: args.length
        params = (args.length,)
    elif args.command == 'uniform':
        assert args.a > 0 and args.b > 0 and args.b > args.a
        dist = lambda: numpy.random.random_integers(args.a, args.b)
        params = (args.a, args.b)
    elif args.command == 'binomial':
        assert args.n > 0 and 0 < args.p < 1
        dist = lambda: numpy.random.binomial(args.n, args.p)
        params = (args.n, args.p)
    else:
        sys.exit(1)

    # Dump the required number of strings to stdout
    print(preamble(args.num, args.command, params))
    for i in itertools.islice(random_strings(dist), args.num):
        print(i)

    sys.exit(0)

if __name__ == '__main__':
    main()
