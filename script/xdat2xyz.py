#!/usr/bin/env python
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


from sys import stdout
from glob import glob
import argparse


def parse_args():
    # define parameterd
    parser = argparse.ArgumentParser('xdat2xyz.py')
    parser.add_argument('--path', type=str, default='./')
    parser.add_argument('--supercell', type=str, default='1 1 1')
    return parser.parse_args()


def distance(vec1, vec2=[0, 0, 0]):
    return norm(sub(vec1, vec2))


def reshape(prev_position, next_position, lattice):
    if prev_position is None: return next_position

    offsets = dot([[dx, dy, dz] for dx in range(-1, 2) for dy in range(-1, 2)
                   for dz in range(-1, 2)], lattice)

    for idx, (coord1, coord2) in enumerate(zip(prev_position, next_position)):
        if distance(coord1, coord2) > 0.8:
            subvec = sub(coord2, coord1)
            superc = [distance(add(subvec, offset)) for offset in offsets]
            closer = sorted(zip(superc, offsets))[0]
            next_position[idx] = add(coord2, closer[1])

    return next_position


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


def make_supercell(supercell, atoms, lattice, positions):
    sx, sy, sz = supercell
    atoms = sx * sy * sz * atoms

    new_position = [
        add(xyz, dot([dx, dy, dz], lattice)) for dx in range(sx)
        for dy in range(sy) for dz in range(sz) for xyz in positions
    ]

    return atoms, new_position


def convert_to_xyz_format(atoms, positions):
    content = '{:d}\nmolecule\n'.format(len(atoms))
    fmt = '{:>2s}  {:12.6F}  {:12.6F}  {:12.6F}\n'
    for (x, y, z), t in zip(positions, atoms):
        content += fmt.format(t, x, y, z)
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

        with open('trajectory.xyz', 'w') as out:

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
                                                lattice)
                            prev_position, readlines = positions, []

                            natoms, positions = make_supercell(
                                supercell, atoms, lattice, positions)
                            out.write(convert_to_xyz_format(natoms, positions))

                            break


if __name__ == '__main__':
    args = parse_args()
    filenames = glob('{:s}/vasprun.xml'.format(args.path))

    for filename in filenames:

        try:
            if len(filenames) > 1:
                stdout.write('# {:s}:\n'.format(filename))
                parse_vasprunxml(filename, args)
                stdout.write('\n')
                stdout.flush()
            else:
                parse_vasprunxml(filename, args)
        except:
            pass
