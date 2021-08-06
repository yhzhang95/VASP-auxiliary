#!/usr/bin/env python
# mail: yhzhang95@seu.edu.cn
# author: yehui zhang / southeast university
# todo: fix h2o molecule
try:
    from xml.etree import cElementTree as ET
except:
    from xml.etree import ElementTree as ET

try:
    from numpy import dot, add, subtract as sub
    from numpy.linalg import norm
except:
    from math import sqrt

    def dot(positions, lattice):
        if type(positions[0]) is list:
            return [[
                sum(pos[row] * lattice[row][col] for row in range(3))
                for col in range(3)
            ] for pos in positions]
        else:
            return [
                sum(positions[row] * lattice[row][col] for row in range(3))
                for col in range(3)
            ]

    def add(vec1, vec2):
        return [i + j for i, j in zip(vec1, vec2)]

    def sub(vec1, vec2):
        return [i - j for i, j in zip(vec1, vec2)]

    def norm(vector):
        return sqrt(sum([i**2 for i in vector]))


from math import acos, pi
import argparse
import datetime


def parse_args():
    # define parameterd
    parser = argparse.ArgumentParser('xdat2arc.py')
    parser.add_argument('--path', type=str, default='./vasprun.xml')
    parser.add_argument('--supercell', type=str, default='1 1 1')
    return parser.parse_args()


def distance(vec1, vec2=[0, 0, 0]):
    return norm(sub(vec1, vec2))


def angle(vec1, vec2):
    vecdot = sum([i * j for i, j in zip(vec1, vec2)])
    result = acos(vecdot / (norm(vec1) * norm(vec2)))
    return result / pi * 180


def lattice_to_cell(lattice):
    normvec = [distance(vec) for vec in lattice]
    combs = ((1, 2), (2, 0), (0, 1))
    angles = [angle(lattice[i], lattice[j]) for i, j in combs]
    return normvec + angles


def reshape(prev_position, next_position, lattice, atoms):
    # if prev_position is None: return next_position
    if prev_position is None:
        return fix_h2o_pbc(next_position, lattice, atoms)

    offsets = dot([[dx, dy, dz] for dx in range(-1, 2) for dy in range(-1, 2)
                   for dz in range(-1, 2)], lattice)
    try:
        offsets = offsets.tolist()
    except:
        pass

    for idx, (coord1, coord2) in enumerate(zip(prev_position, next_position)):
        if distance(coord1, coord2) > 0.8:
            subvec = sub(coord2, coord1)
            superc = [distance(add(subvec, offset)) for offset in offsets]
            closer = sorted(zip(superc, offsets))[0]
            next_position[idx] = add(coord2, closer[1])

    return next_position


def fix_h2o_pbc(position, lattice, atoms):
    offsets = dot([[dx, dy, dz] for dx in range(-1, 2) for dy in range(-1, 2)
                   for dz in range(-1, 2)], lattice)
    try:
        offsets = offsets.tolist()
    except:
        pass

    o_position = [pos for pos, symbol in zip(position, atoms) if symbol == 'O']
    h_position = [(idx, pos)
                  for idx, (pos, symbol) in enumerate(zip(position, atoms))
                  if symbol == 'H']

    for idx, h_pos in h_position:
        oh_dist = min([distance(o_pos, h_pos) for o_pos in o_position])
        if oh_dist <= 1.2: continue

        for offset in offsets:
            oh_dist = min(
                [distance(o_pos, add(h_pos, offset)) for o_pos in o_position])
            if oh_dist <= 1.2:
                position[idx] = add(h_pos, offset)
                break

    return position


def parse_atoms(fb):
    for line in fb:
        readlines = []
        if '<set>' in line:
            readlines.append(line)
            for line in fb:
                readlines.append(line)
                if '</set>' in line:
                    break
            break

    root = ET.fromstring(''.join(readlines))
    atoms = [[val.text.strip() for val in child] for child in root]
    atoms = [child[0] for child in atoms]

    return atoms


def parse_calculation(calculation):
    # xml format parsed by elementtree
    root = ET.fromstring(''.join(calculation))

    for child in root:

        if child.tag == 'structure':
            for ichild in child:
                if ichild.tag == 'crystal':
                    for iichild in ichild:
                        if iichild.get('name') == 'basis':
                            lattice = [[float(i) for i in lline.text.split()]
                                       for lline in iichild]

                elif ichild.get('name') == 'positions':
                    positions = [[float(i) for i in pline.text.split()]
                                 for pline in ichild]

    positions = dot(positions, lattice)

    return lattice, positions


def make_supercell(supercell, atoms, cell, lattice, positions):
    sx, sy, sz = supercell
    atoms = sx * sy * sz * atoms
    cell[0], cell[1], cell[2] = cell[0] * sx, cell[1] * sy, cell[2] * sz

    new_position = [
        add(xyz, dot([dx, dy, dz], lattice)) for dx in range(sx)
        for dy in range(sy) for dz in range(sz) for xyz in positions
    ]

    return atoms, cell, new_position


def convert_to_arc_format(atoms, cell, positions):
    content = '{:80.4F}\n'.format(0.)
    content += '!DATE     {:s}\n'.format(datetime.datetime.now().isoformat())
    na, nb, nc, alpha, gamma, beta = cell
    fmt = 'PBC' + '{:10.4F}' * 6 + '\n'
    content += fmt.format(na, nb, nc, alpha, gamma, beta)

    fmt = '{:>2s}{:16.9F}{:16.9F}{:16.9F}{:>5s}{:2d}{:>8s}{:>8s}{:7.3F}\n'
    for (x, y, z), t in zip(positions, atoms):
        content += fmt.format(t, x, y, z, 'XXXX', 1, 'xx', t, 0)
    content += 'end\nend\n'

    return content


def parse_vasprunxml(filename, args):

    # get argv
    supercell = [int(val) for val in args.supercell.split()]

    with open(filename, 'r') as fb:
        # get atoms information
        for line in fb:
            if '<array name="atoms" >' in line:
                atoms = parse_atoms(fb)
                break

        arcname = filename.replace('vasprun.xml', 'trajectory.arc')
        with open(arcname, 'w') as out:
            out.write('!BIOSYM archive 3\nPBC=ON\n')

            # get last calculation information
            prev_position, readlines = None, []
            for line in fb:
                if '<calculation>' in line:
                    readlines.append(line)
                    for line in fb:
                        readlines.append(line)
                        if '</calculation>' in line:

                            lattice, positions = parse_calculation(readlines)
                            positions = reshape(prev_position, positions,
                                                lattice, atoms)
                            prev_position, readlines = positions, []

                            cell = lattice_to_cell(lattice)
                            natoms, cell, positions = make_supercell(
                                supercell, atoms, cell, lattice, positions)
                            out.write(
                                convert_to_arc_format(natoms, cell, positions))

                            break


if __name__ == '__main__':
    args = parse_args()

    try:
        parse_vasprunxml(args.path, args)
    except Exception as e:
        print("type error: " + str(e))
