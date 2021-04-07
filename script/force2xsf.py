#!/usr/bin/env python
try:
    from xml.etree import cElementTree as ET
except:
    from xml.etree import ElementTree as ET

try:
    from numpy import dot
except:

    def dot(positions, lattice):
        return [[
            sum(pos[row] * lattice[row][col] for row in range(3))
            for col in range(3)
        ] for pos in positions]


from sys import stdout
from glob import glob
from math import sqrt
import argparse
import re


def parse_args():
    # define parameterd
    parser = argparse.ArgumentParser('force2xsf.py')
    parser.add_argument('--path', type=str, default='./')
    parser.add_argument('--all', type=bool, default=False)
    return parser.parse_args()


def data_to_xsffile(lattice, atoms, positions, forces, stress):
    # output to contcar.xsf file
    with open('contcar.xsf', 'w') as fb:
        # output title
        fb.write('CRYSTAL\n')

        # output lattice
        fb.write('PRIMVEC\n')
        fmt = '    {:14.8F} {:14.8F} {:14.8F}\n'
        for x, y, z in lattice:
            fb.write(fmt.format(x, y, z))

        # output coords
        fb.write('PRIMCOORD\n{:d} 1\n'.format(len(atoms)))
        fmt = '{:>3s}' + ' {:14.8F}' * 6 + '\n'
        for atom, (x, y, z), (dx, dy, dz) in zip(atoms, positions, forces):
            fb.write(fmt.format(atom, x, y, z, dx, dy, dz))

        # output energy
        fb.write('PRIMFORCE\n')
        fb.write('    {:14.8F}\n'.format(
            max(sqrt(x**2 + y**2 + z**2) for x, y, z in forces)))

        # output stress
        fb.write('PRIMSTRESS\n')
        fmt = '    {:14.8F} {:14.8F} {:14.8F}\n'
        for v1, v2, v3 in stress:
            fb.write(fmt.format(v1, v2, v3))


def parse_ediffg(fb):
    for line in fb:
        if 'EDIFFG' in line:
            EDIFFG = re.search('<i.*>(.*?)</i>', line).group(1)
            return float(EDIFFG)


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


def parse_selective(fb):
    for line in fb:
        readlines = []
        if 'name="positions"' in line:
            readlines.append(line)
            for line in fb:
                readlines.append(line)
                if '</varray>' in line: break

            root = ET.fromstring(''.join(readlines))
            selective = [True for _ in root]
            break

    for line in fb:
        readlines = []
        if 'name="selective"' in line:
            readlines.append(line)
            for line in fb:
                readlines.append(line)
                if '</varray>' in line: break

            root = ET.fromstring(''.join(readlines))
            selective = ['F' not in child.text for child in root]
            break

        elif '</structure>' in line:
            break

    return selective


def parse_calculation(lastcal, EDIFFG, selective, logicalall):
    # xml format parsed by elementtree
    root = ET.fromstring(''.join(lastcal))

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

        elif child.get('name') == 'forces':
            forces = []
            if not logicalall:
                for fline, flag in zip(child, selective):

                    if flag is not True:
                        forces.append([0., 0., 0.])
                        continue

                    force = [float(i) for i in fline.text.split()]
                    normforce = sqrt(sum(f**2 for f in force))
                    if EDIFFG < 0 and normforce >= -EDIFFG:
                        forces.append(force)
                    else:
                        forces.append([0., 0., 0.])
            else:
                forces = [[float(i) for i in fline.text.split()]
                          for fline in child]

        elif child.get('name') == 'stress':
            stress = [[float(i) for i in sline.text.split()]
                      for sline in child]

    positions = dot(positions, lattice)

    return lattice, positions, forces, stress


def parse_vasprunxml(filename, logicalall):

    with open(filename, 'r') as fb:

        # get ediffg value
        for line in fb:
            if '<separator name="ionic" >' in line:
                EDIFFG = parse_ediffg(fb)
                break

        # get atoms information
        for line in fb:
            if '<array name="atoms" >' in line:
                atoms = parse_atoms(fb)
                break

        # get selective information
        for line in fb:
            if '<structure name="initialpos" >' in line:
                selective = parse_selective(fb)
                break

        # get last calculation information
        lastcal, readlines = None, []
        for line in fb:
            if '<calculation>' in line:
                readlines.append(line)
                for line in fb:
                    readlines.append(line)
                    if '</calculation>' in line:
                        lastcal, readlines = readlines, []
                        break

        lattice, positions, forces, stress = parse_calculation(
            lastcal, EDIFFG, selective, logicalall)

        data_to_xsffile(lattice, atoms, positions, forces, stress)


if __name__ == '__main__':
    args = parse_args()
    filenames = glob('{:s}/vasprun.xml'.format(args.path))

    for filename in filenames:
        try:
            if len(filenames) > 1:
                stdout.write('# {:s}:\n'.format(filename))
                parse_vasprunxml(filename, args.all)
                stdout.write('\n')
                stdout.flush()
            else:
                parse_vasprunxml(filename, args.all)
        except:
            pass
