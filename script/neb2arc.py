#!/usr/bin/env python
import sys, datetime, math, glob, os

add = lambda a, b: [i + j for i, j in zip(a, b)]
dot = lambda a, b: sum([i * j for i, j in zip(a, b)])
dist2 = lambda a, b=[0, 0, 0]: sum([(i - j)**2 for i, j in zip(a, b)])
dist = lambda a, b=[0, 0, 0]: math.sqrt(dist2(a, b))
angle = lambda a, b: math.acos(dot(a, b) / (dist(a) * dist(b))) / math.pi * 180
str2f = lambda strs: [float(i) for i in strs.split()[:3]]


def frac2xyz(line, lat):
    vec = str2f(line) if type(line) == str else line
    return [sum([vec[r] * lat[r][c] for r in range(3)]) for c in range(3)]


def lat2cell(lat):
    head = [dist(line) for line in lat]
    tail = [angle(lat[i], lat[j]) for i, j in ((1, 2), (2, 0), (0, 1))]
    return head + tail


def reshape(font, back, lat):
    return_list = []
    offsets = [
        frac2xyz([dx, dy, dz], lat) for dx in range(-1, 2)
        for dy in range(-1, 2) for dz in range(-1, 2)
    ]

    for coord1, coord2 in zip(font, back):
        if dist2(coord1, coord2) <= 0.64:
            return_list.append(coord2)
        else:
            subvec = [i - j for i, j in zip(coord2, coord1)]
            superc = [dist2(add(subvec, offset)) for offset in offsets]
            closer = sorted(zip(superc, offsets))[0]
            return_list.append(add(coord2, closer[1]))

    return return_list


def xyz2str(xyzs, cell, atoms):
    content = '%80.4f\n' % 0
    content += '!DATE     %s\n' % datetime.datetime.now().isoformat()
    content += 'PBC%10.4f%10.4f%10.4f%10.4f%10.4f%10.4f\n' % tuple(cell)
    for (x, y, z), t in zip(xyzs, atoms):
        content += '%2s%16.9f%16.9f%16.9f%5s%2d%8s%8s%7.3f\n' % (
            t, x, y, z, 'XXXX', 1, 'xx', t, 0)
    content += 'end\nend\n'
    return content


def interp(newline, cells):
    import numpy as np
    from scipy.interpolate import interp1d

    inset_num = int(sys.argv[2]) if len(sys.argv) > 2 else 10
    cells, newline = np.array(cells), np.array(newline)

    length = len(cells)
    x_order = np.arange(length)
    xx_order = np.linspace(0, length - 1, (inset_num + 1) * length + 1)

    fn_cell = interp1d(x_order, cells.T, kind='cubic')
    fn_newline = interp1d(x_order, newline.T, kind='cubic')

    _cells, _newline = fn_cell(xx_order).T, fn_newline(xx_order).T
    return _newline, _cells


# find all folders
folders = [name for name in os.listdir('./') if os.path.isdir('./%s' % name)]
folders = [name for name in folders if name.isdigit()]
folders = sorted(folders, key=lambda x: int(x))
source = []
for name in folders:
    path = os.path.join('./', name, 'CONTCAR')
    if os.path.isfile(path):
        source.append(open(path, 'r').readlines())
    else:
        path = os.path.join('./', name, 'POSCAR')
        source.append(open(path, 'r').readlines())

# parse header
init_state = source[0]
atom_type = init_state[5].split()
atom_num = [int(i) for i in init_state[6].split()]
atoms = [t for t, n in zip(atom_type, atom_num) for _ in range(n)]

idxs = [
    idx for state in source for idx, line in enumerate(state)
    if 'Direct' in line
]

# convert to xyz file
lats = [[str2f(line) for line in state[2:5]] for state in source]
cells = [lat2cell(lat) for lat in lats]
coords = [[frac2xyz(line, lat) for line, _ in zip(state[idx + 1:], atoms)]
          for state, lat, idx in zip(source, lats, idxs)]

# make offset for view
newline = []
for idx, (xyzs, lat) in enumerate(zip(coords, lats)):
    if idx == 0: newline.append(xyzs)
    else: newline.append(reshape(newline[-1], xyzs, lat))

arg = sys.argv[1] if len(sys.argv) > 1 else None
if arg == '--interp': newline, cells = interp(newline, cells)

# make supercell
filename = 'neb.arc'
with open(filename, 'w') as outfile:
    outfile.write('!BIOSYM archive 3\nPBC=ON\n')
    for xyzs, cell in zip(newline, cells):
        outfile.write(xyz2str(xyzs, cell, atoms))
