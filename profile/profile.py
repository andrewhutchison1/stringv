#!/usr/bin/env python3
from matplotlib.pyplot import cm
import numpy as np
import sys
import csv
import argparse

CLOCKS_PER_SEC = 1000000

class ProfileData(object):
    """ Represents numeric profiling data. """
    def __init__(self, csv_filename=None):
        # The number of data series that will be plotted
        self._n_series = 0
        # The list of series labels
        self._labels = []
        # The list of series data (list of list of floats)
        self._series = []

        # If a filename is given, open it. Otherwise, read from stdin.
        if csv_filename is not None:
            with open(csv_filename, 'r') as f:
                self._read_data(f)
        else:
            self._read_data(sys.stdin)

    def _read_data(self, f):
        """ Reads the data stored in the given *open* file """
        strings = f.read().split('\n')
        string_data = [x for x in csv.reader(strings) if x != []]

        # Read the labels, which is always the first element of string_data.
        # The number of series is always determined from the length of the
        # labels list.
        self._labels = string_data[0]
        self._n_series = len(self._labels)

        # Now, the data are the remaining elements of string_data, but are
        # currently stored as strings. We must parse them into floats to be
        # stored in self._series.
        for i in range(self._n_series):
            self._series.append([float(e[i]) for e in string_data[1:]])

    def n_series(self):
        """ Returns the number of series. """
        return self._n_series

    def independent_label(self):
        """ Returns the dependent variable's label """
        return self._labels[0]

    def dependent_labels(self):
        """ Returns a tuple of the dependent variable series labels """
        return tuple(self._labels[1:])

    def independent(self):
        """ Returns a list of the independent variable's data """
        return self._series[0]

    def dependent(self):
        """ Returns a tuple of the dependent variables data series """
        return tuple(self._series[1:])


def plot_colours(n):
    """
    Returns up to n unique colours as they are distributed linearly throughout
    the rainbow.
    """
    return cm.rainbow(np.linspace(0, 1, n))

def profile_arg_parser():
    """
    Defines and returns an argument parser for parsing the options of a
    profile plot.
    """
    parser = argparse.ArgumentParser(description='Plots csv profile data')
    # The output file (positional)
    parser.add_argument('output_file', help='The name of the output file', \
            type=str)
    # The input file (optional)
    parser.add_argument('-i', '--input', help='The name of the input csv '
            'file. Defaults to stdin if no value is given', nargs=1, type=str)
    # The output file format (optional)
    parser.add_argument('-f', '--format', help='The output file format. ',
            nargs=1, default='svg', choices=['png', 'svg'])
    # The plot title
    parser.add_argument('-t', '--title', help='The title of the plot', \
            nargs=1, required=True)
    # The axis titles
    parser.add_argument('-x', '--xlabel', help='The label of the horizontal '
            'axis.', nargs=1, required=True)
    parser.add_argument('-y', '--ylabel', help='The label of the vertical '
            'axis.', nargs=1, required=True)
    return parser
