#!/usr/bin/env python
import argparse


def parse_args():
    # define parameterd
    parser = argparse.ArgumentParser('xdat2arc.py')
    parser.add_argument('--filename', type=str, default='LOCPOT')
    parser.add_argument('--axis', type=str, default='z')
    return parser.parse_args()


try:
    import numpy as np

    def parse_locpot(args):
        # Get header of LOCPOT
        locpot = open(args.filename, 'r').readlines()
        lattice = [line.split() for line in locpot[2:5]]
        lattice = np.array(lattice, dtype='float')

        # get some parameter
        sidx = 9 + sum([int(i) for i in locpot[6].split()])
        ngx, ngy, ngz = [int(i) for i in locpot[sidx].split()]
        totoal = ngz * ngy * ngx

        # read raw data
        mdata = [float(i) for line in locpot[sidx + 1:] for i in line.split()]
        ispin = int(len(mdata) / totoal)
        mdata = np.array(mdata[:totoal] + mdata[-totoal:])
        mdata = mdata.reshape(2, ngz, ngy, ngx)

        ngn = {'x': ngx, 'y': ngy, 'z': ngz}
        lat = {'x': lattice[0], 'y': lattice[1], 'z': lattice[2]}

        length = lambda line: np.sqrt(sum([i**2 for i in line]))
        sdata = np.linspace(0, length(lat[args.axis]), ngn[args.axis])
        if ispin == 1:
            if args.axis == 'x':
                sdata = [[val, np.mean(mdata[0, :, :, x])]
                         for x, val in enumerate(sdata)]
            elif args.axis == 'y':
                sdata = [[val, np.mean(mdata[0, :, y, :])]
                         for y, val in enumerate(sdata)]
            elif args.axis == 'z':
                sdata = [[val, np.mean(mdata[0, z, :, :])]
                         for z, val in enumerate(sdata)]

            with open('vplanar.dat', 'w') as fb:
                fb.write('{:>8s} {:>12s}\n'.format(args.axis, 'Energy'))
                for coord, data in sdata:
                    fb.write('{:8.4F} {:12.6F}\n'.format(coord, data))

        elif ispin == 2:
            if args.axis == 'x':
                sdata = [[
                    val,
                    np.mean(mdata[0, :, :, x]),
                    np.mean(mdata[1, :, :, x])
                ] for x, val in enumerate(sdata)]
            elif args.axis == 'y':
                sdata = [[
                    val,
                    np.mean(mdata[0, :, y, :]),
                    np.mean(mdata[1, :, y, :])
                ] for y, val in enumerate(sdata)]
            elif args.axis == 'z':
                sdata = [[
                    val,
                    np.mean(mdata[0, z, :, :]),
                    np.mean(mdata[1, z, :, :])
                ] for z, val in enumerate(sdata)]

            with open('vplanar.dat', 'w') as fb:
                fb.write('{:>8s} {:>12s} {:>12s}\n'.format(
                    args.axis, 'Energy_up', 'Energy_down'))
                for coord, up, down in sdata:
                    fb.write('{:8.4F} {:12.6F} {:12.6F}\n'.format(
                        coord, up, down))

except:
    from math import sqrt

    def parse_locpot(args):
        # Get header of LOCPOT
        locpot = open(args.filename, 'r').readlines()
        lattice = [[float(i) for i in line.split()] for line in locpot[2:5]]

        # get some parameter
        sidx = 9 + sum([int(i) for i in locpot[6].split()])
        ngx, ngy, ngz = [int(i) for i in locpot[sidx].split()]
        totoal = ngz * ngy * ngx

        # read raw data
        mdata = [float(i) for line in locpot[sidx + 1:] for i in line.split()]
        ispin = int(len(mdata) / totoal)

        ngn = {'x': ngx, 'y': ngy, 'z': ngz}
        lat = {'x': lattice[0], 'y': lattice[1], 'z': lattice[2]}

        length = lambda line: sqrt(sum([i**2 for i in line]))
        step = length(lat[args.axis]) / (ngn[args.axis] - 1)
        npts = totoal // ngn[args.axis]

        if ispin == 1:
            upsource = iter(mdata[:totoal])
            updata = [[[next(upsource) for _ in range(ngx)]
                       for _ in range(ngy)] for _ in range(ngz)]

            if args.axis == 'x':
                sdata = [[idx * step,
                          sum(sum(updata[:][:][idx], [])) / npts]
                         for idx in range(ngn[args.axis])]

            elif args.axis == 'y':
                sdata = [[idx * step,
                          sum(sum(updata[:][idx][:], [])) / npts]
                         for idx in range(ngn[args.axis])]

            elif args.axis == 'z':
                sdata = [[idx * step,
                          sum(sum(updata[idx][:][:], [])) / npts]
                         for idx in range(ngn[args.axis])]

            with open('vplanar.dat', 'w') as fb:
                fb.write('{:>8s} {:>12s}\n'.format(args.axis, 'Energy'))
                for coord, data in sdata:
                    fb.write('{:8.4F} {:12.6F}\n'.format(coord, data))

        elif ispin == 2:

            upsource = iter(mdata[:totoal])
            updata = [[[next(upsource) for _ in range(ngx)]
                       for _ in range(ngy)] for _ in range(ngz)]

            dnsource = iter(mdata[-totoal:])
            dndata = [[[next(dnsource) for _ in range(ngx)]
                       for _ in range(ngy)] for _ in range(ngz)]

            if args.axis == 'x':
                sdata = [[
                    idx * step,
                    sum(sum(updata[:][:][idx], [])) / npts,
                    sum(sum(dndata[:][:][idx], [])) / npts
                ] for idx in range(ngn[args.axis])]

            elif args.axis == 'y':
                sdata = [[
                    idx * step,
                    sum(sum(updata[:][idx][:], [])) / npts,
                    sum(sum(dndata[:][idx][:], [])) / npts
                ] for idx in range(ngn[args.axis])]

            elif args.axis == 'z':
                sdata = [[
                    idx * step,
                    sum(sum(updata[idx][:][:], [])) / npts,
                    sum(sum(dndata[idx][:][:], [])) / npts
                ] for idx in range(ngn[args.axis])]

            with open('vplanar.dat', 'w') as fb:
                fb.write('{:>8s} {:>12s} {:>12s}\n'.format(
                    args.axis, 'Energy_up', 'Energy_down'))
                for coord, up, down in sdata:
                    fb.write('{:8.4F} {:12.6F} {:12.6F}\n'.format(
                        coord, up, down))


if __name__ == '__main__':
    args = parse_args()
    parse_locpot(args)
