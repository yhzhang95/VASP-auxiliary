#!/usr/bin/env python
try:
    from xml.etree import cElementTree as ET
except:
    from xml.etree import ElementTree as ET

from itertools import product
from sys import argv
import numpy as np


class Structure(object):
    __slots__ = ('lattice', 'position', 'cartesian', 'dist')

    def __init__(self, calculation):
        # xml format parsed by elementtree
        root = ET.fromstring(''.join(calculation))

        for child in root:
            if child.tag == 'structure':
                for ichild in child:
                    if ichild.tag == 'crystal':
                        for iichild in ichild:
                            if iichild.get('name') == 'basis':
                                lattice = [
                                    lline.text.split() for lline in iichild
                                ]

                    elif ichild.get('name') == 'positions':
                        positions = [pline.text.split() for pline in ichild]

        self.lattice = np.array(lattice, dtype='float')
        self.position = np.array(positions, dtype='float')
        self.cartesian = np.dot(self.position, self.lattice)

    def remove_pbc(self, previous):
        deltas = self.cartesian - previous.cartesian
        norms = np.sqrt(np.sum(deltas**2, axis=-1))
        if np.all(norms < 1): return

        offset = product(range(-2, 3), range(-2, 3), range(-2, 3))
        offset = np.dot(np.array(list(offset)), self.lattice)

        for idx, (delta, flag) in enumerate(zip(deltas, norms < 1)):
            if flag: continue
            newpos = offset + delta
            norms = np.sqrt(np.sum(newpos**2, axis=-1))
            key = np.argmin(norms)
            self.cartesian[idx] = self.cartesian[idx] + offset[key]

    def distance(self, idx1, idx2):
        offset = product(range(-2, 3), range(-2, 3), range(-2, 3))
        offset = np.dot(np.array(list(offset)), self.lattice)

        dists = [
            np.linalg.norm(self.cartesian[idx1] - self.cartesian[idx2] - dxyz)
            for dxyz in offset
        ]
        self.dist = min(dists)


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


def main(idx1, idx2):
    # parse vasprun.xml file
    with open('vasprun.xml', 'r') as fb:
        # get atoms information
        for line in fb:
            if '<array name="atoms" >' in line:
                atoms = parse_atoms(fb)
                break

        # get structures information
        structures, readlines = [], []
        for line in fb:
            if '<calculation>' in line:
                readlines.append(line)
                for line in fb:
                    readlines.append(line)
                    if '</calculation>' in line:
                        try:
                            structures.append(Structure(readlines))
                        except:
                            print(readlines)
                        readlines = []
                        break

     # remove the effect of pbc
    for prev, curr in zip(structures, structures[1:]):
        curr.remove_pbc(prev)

    # calculate squared displacement
    for st in structures:
        st.distance(idx1, idx2)

    # output to files
    with open('dist.dat', 'w') as fb:
        # output header
        label = '{:s}-{:s}-dist'.format(atoms[idx1], atoms[idx2])
        fb.write('{:>8s} {:>12s}\n'.format('Time', label))

        # output data by time
        for idx, st in enumerate(structures):
            fb.write('{:8d} {:12.6F}\n'.format(idx, st.dist))


if __name__ == "__main__":
    if len(argv) < 3:
        print('[usage]: python dist.py idx1 idx2')
        exit()

    idx1, idx2 = int(argv[1]) - 1, int(argv[2]) - 1
    main(idx1, idx2)
