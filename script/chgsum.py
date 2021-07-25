#!/usr/bin/env python
import os, sys, argparse, numpy as np

parser = argparse.ArgumentParser()
parser.add_argument('-o',
                    '--output',
                    nargs=1,
                    help='<Required> filename',
                    default='chgdiff')
parser.add_argument('-f',
                    '--filename',
                    nargs=2,
                    action='append',
                    metavar=('filename', 'coeff'),
                    help='<Required> filename coefficient',
                    required=True)


class chgcar(object):
    def __init__(self, filename, coeff):
        self.size = None
        self.header = None
        self.data = None
        self.grid = None
        self.ncl_all = False

        self.__file_exist(filename, coeff)
        self.__read_head(filename, coeff, check=False)

    def append(self, filename, coeff):
        self.__file_exist(filename, coeff)
        self.__read_head(filename, coeff, check=True)

    def __file_exist(self, filename, coeff):
        sys.stdout.write('Parsing [filename]: %s [coeff]: %g ... ' %
                         (filename, coeff))
        sys.stdout.flush()
        if not os.path.isfile(filename):
            sys.stdout.write('failed.\n')
            exit(1)

    def __read_head(self, filename, coeff, check):
        # read file
        source = open(filename, 'r').readlines()
        natom = sum([int(n) for n in source[6].split()]) + 8
        grid = [int(ng) for ng in source[natom + 1].split()]
        grid = grid[::-1]
        if check:
            if self.grid != grid:
                sys.stderr.write('NGX, NGY, NGZ not fit well. Please check.\n')
                exit(2)

        # read charge data
        grid_line = source[natom + 1]
        grid_index = [
            idx for idx, line in enumerate(source) if line == grid_line
        ] + [None]
        zones = [
            source[start_idx + 1:stop_idx]
            for start_idx, stop_idx in zip(grid_index[:-1], grid_index[1:])
        ]

        # set type of system
        # if size == 4: self.type = 'non collinear'
        # elif size == 2: self.type = 'spin'
        # elif size == 1: self.type = 'normal'
        size = len(zones)
        if check:
            if self.size != size:
                sys.stderr.write('CHGCAR type not fit well. Please check.\n')
                exit(2)

        # read line and save to numbers
        nx, ny, nz = grid
        data = np.zeros((size, nx, ny, nz))
        for inum, zone in enumerate(zones):
            cut_idx = None
            for idx, line in enumerate(zone):
                if 'augmentation' in line:
                    cut_idx = idx
                    break
            data[inum] += np.array(
                [i for line in zone[:cut_idx] for i in line.split()],
                dtype='float').reshape(tuple(grid))

        sys.stdout.write('done\n')
        sys.stdout.flush()
        if check:
            self.data += data * coeff
        else:
            self.header, self.data = source[:natom], data * coeff
            self.grid, self.size = grid, size

        if self.size == 2:
            self.spin_up = (self.data[0] + self.data[1]) / 2
            self.spin_down = (self.data[0] - self.data[1]) / 2

    def __com_out(self, filename, data):
        filename = filename + '.vasp'
        sys.stdout.write('Writing [filename]: %s ... ' % filename)
        sys.stdout.flush()

        with open(filename, 'w') as out:
            out.writelines(self.header)
            out.write('\n%5d%5d%5d\n' % tuple(self.grid[::-1]))

            for idx, value in enumerate(data.flatten()):
                out.write(' %+11.4E' % value)
                if idx % 10 == 9: out.write('\n')

        sys.stdout.write('done\n')
        sys.stdout.flush()

    def output(self, filename):
        if self.size == 1:
            self.__com_out(filename, self.data[0])
        elif self.size == 2:
            self.__com_out(filename + '_all', self.data[0])
            self.__com_out(filename + '_up', self.spin_up)
            self.__com_out(filename + '_down', self.spin_down)
        elif self.size == 4:
            self.__com_out(filename + '_all', self.data[0])
            if self.ncl_all:
                self.__com_out(filename + '_x', self.data[1])
                self.__com_out(filename + '_y', self.data[2])
                self.__com_out(filename + '_z', self.data[3])


if __name__ == '__main__':
    # get config
    args = parser.parse_args()
    filename_list = args.filename
    names = [name for name, _ in filename_list]
    coeffs = [float(coeff) for _, coeff in filename_list]

    # parse file
    base = chgcar(names[0], coeffs[0])
    for name, coeff in zip(names[1:], coeffs[1:]):
        base.append(name, coeff)
    base.output(args.output)
