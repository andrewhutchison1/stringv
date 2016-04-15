#!/usr/bin/env python3
import sys
import matplotlib.pyplot as plt

import profile

def main():
    args = vars(profile.profile_arg_parser().parse_args())
    data = profile.ProfileData(args['input'])
    colours = profile.plot_colours(data.n_series())

    figure = plt.figure()
    axes = plt.subplot(111)

    for l,d,c in zip(data.dependent_labels(), data.dependent(), colours):
        axes.plot(data.independent(), d, marker='o', color=c, mew=0, label=l)

    legend = axes.legend(loc='center right', bbox_to_anchor=(1.3, 0.5))
    axes.set_xlabel(*args['xlabel'])
    axes.set_ylabel(*args['ylabel'])
    axes.set_title(*args['title'])

    figure.savefig(args['output_file'], format=args['format'][0], \
            bbox_extra_artists=(legend,), bbox_inches='tight')

if __name__ == '__main__':
    main()
