#!/usr/bin/env python3
import numpy
import string
import random
import itertools
import argparse
import sys

class LengthDistribution(object):
    """
    Base class for length distributions.
    """
    def __init__(self):
        # Characters to sample from
        self._CHARS = string.ascii_lowercase + string.ascii_uppercase + \
                string.digits

    def sample(self):
        """
        Samples the underlying length distribution to retrieve a length L, and
        generates a string of length L consisting of characters drawn from the
        space self._CHARS.
        """
        while True:
            yield ''.join(random.choice(self._CHARS) for _ in range( \
                    self._dist()))

    def _dist(self):
        """
        The actual length distribution function. This function should be
        implemented in a derived class, and should return an integer quantity
        that is a sampling of the underlying distribution represented by the
        derived instance.
        """
        raise NotImplementedError('Must be called in a derived instance')

class IdenticalLengthDistribution(LengthDistribution):
    """
    Generates strings of identical length, where the length is given by the
    mean parameter of the base class.
    """
    def __init__(self, length):
        super().__init__()
        self._length = length

    def _dist(self):
        return self._length

class UniformLengthDistribution(LengthDistribution):
    """
    Generates strings of length L, where L is distributed uniformly in the
    range [lower, upper].
    """
    def __init__(self, lower, upper):
        super().__init__()
        self._lower = lower
        self._upper = upper

    def _dist(self):
        return numpy.random.random_integers(self._lower, self._upper)

class BinomialLengthDistribution(LengthDistribution):
    """
    Generates strings of length L, where L is distributed according to the
    binomial distribution with parameters n and p.
    """
    def __init__(self, n, p):
        super().__init__()
        self._n = n
        self._p = p

    def _dist(self):
        return numpy.random.binomial(self._n, self._p)

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
    assert args.num > 0

    if args.command == 'identical':
        dist = IdenticalLengthDistribution(args.length)
    elif args.command == 'uniform':
        dist = UniformLengthDistribution(args.a, args.b)
    elif args.command == 'binomial':
        dist = BinomialLengthDistribution(args.n, args.p)
    else:
        sys.exit(1)

    # Dump the required number of strings to stdout
    for i in itertools.islice(dist.sample(), args.num):
        print(i)

    sys.exit(0)

if __name__ == '__main__':
    main()
