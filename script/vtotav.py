#!/usr/bin/env python
from sys import stdout, argv
import numpy as np

# Get header of LOCPOT
axis = argv[2].lower() if len(argv) > 2 else 'z'
filename = argv[1] if len(argv) > 1 else 'LOCPOT'
locpot = open(filename, 'r').readlines()
lattice = np.array([line.split() for line in locpot[2:5]], dtype='float')

# get some parameter
atom_num = [int(i) for i in locpot[6].split()]
sidx = 9 + sum(atom_num)
ngx, ngy, ngz = [int(i) for i in locpot[sidx].split()]
totoal = ngz * ngy * ngx

# read raw data
mdata = [float(i) for line in locpot[sidx + 1:] for i in line.split()]
ispin = int(len(mdata) / totoal)
mdata = np.array(mdata[:totoal] + mdata[-totoal:]).reshape(2, ngz, ngy, ngx)

length = lambda line: np.sqrt(sum([i**2 for i in line]))
ngn = {'x': ngx, 'y': ngy, 'z': ngz}
lat = {'x': lattice[0], 'y': lattice[1], 'z': lattice[2]}

sdata = np.linspace(0, length(lat[axis]), ngn[axis])
if ispin == 1:
    if axis == 'x':
        sdata = [(val, np.mean(mdata[0, :, :, x]))
                 for x, val in enumerate(sdata)]
    elif axis == 'y':
        sdata = [(val, np.mean(mdata[0, :, y, :]))
                 for y, val in enumerate(sdata)]
    elif axis == 'z':
        sdata = [(val, np.mean(mdata[0, z, :, :]))
                 for z, val in enumerate(sdata)]

    with open('vplanar.dat', 'w') as fb:
        fb.write('%8s %12s\n' % (axis, 'Energy'))
        for data in sdata:
            fb.write('%8.4F %12.6F\n' % data)

elif ispin == 2:
    if axis == 'x':
        sdata = [(val, np.mean(mdata[0, :, :, x]), np.mean(mdata[1, :, :, x]))
                 for x, val in enumerate(sdata)]
    elif axis == 'y':
        sdata = [(val, np.mean(mdata[0, :, y, :]), np.mean(mdata[1, :, y, :]))
                 for y, val in enumerate(sdata)]
    elif axis == 'z':
        sdata = [(val, np.mean(mdata[0, z, :, :]), np.mean(mdata[1, z, :, :]))
                 for z, val in enumerate(sdata)]

    with open('vplanar.dat', 'w') as fb:
        fb.write('%8s %12s %12s\n' % (axis, 'Energy_up', 'Energy_down'))
        for data in sdata:
            fb.write('%8.4F %12.6F %12.6F\n' % data)
