##!/usr/bin/env python
from sys import argv
from math import exp

# Pre
outcar = 'OUTCAR'
flag = 0  # 0: sort by type, 1: sort by Atom, 2: sort by select
select = [[1, 2, 3], [5, 7, 8]]
Gauss_C = 10
plot = [0, 2000, 5001]  # start, end, number

# Read Atom number and type
OUTCAR = open(outcar, "r").readlines()
AtomN, AtomType, index = 0, 0, 0
for line in OUTCAR:
    if "ions per type" in line:
        AtomN = [int(i) for i in line.split()[4:]]
        break
for i, line in enumerate(OUTCAR[:200]):
    if "INCAR" in line:
        AtomType = [tp.split()[2] for tp in OUTCAR[i + 1:i + 1 + len(AtomN)]]
        break
for i, line in enumerate(OUTCAR):
    if "Eigenvectors and eigenvalues of the dynamical matrix" in line:
        index = i + 4
        break

# Get Local
NUMBER, LOC, WAVENUM = sum(AtomN), [], []

# flag setting
if flag == 0:
    Atom = [AtomType[i] for i in xrange(len(AtomN)) for j in xrange(AtomN[i])]
elif flag == 1:
    Atom = [
        "%s%d" % (AtomType[i], j + 1) for i in xrange(len(AtomN))
        for j in xrange(AtomN[i])
    ]
    AtomType = Atom
elif flag == 2:
    AtomType = ["Type%d" % i for i in xrange(len(select))] + ["etc"]
    Atom = ["etc"] * NUMBER
    for i in xrange(NUMBER):
        for j in xrange(len(select)):
            if i + 1 in select[j]:
                Atom[i] = "Type%d" % j

for i in xrange(3 * NUMBER):
    START, END = index + (NUMBER + 3) * i, index + (NUMBER + 3) * (i + 1)
    wave_tmp, loc_tmp = OUTCAR[START].split()[-4], {i: 0 for i in AtomType}
    for j, line in enumerate(OUTCAR[START + 2:END - 1]):
        loc_tmp[Atom[j]] += sum([float(i)**2 for i in line.split()[3:6]])
    LOC += [loc_tmp]
    WAVENUM += [float(wave_tmp)]

origin = open("origin.dat", "w")
origin.write("%12s" % 'WAVENUMBER')
for j in AtomType:
    origin.write("%12s" % j)
origin.write("\n")
for i in xrange(3 * NUMBER):
    origin.write("%12.5f" % WAVENUM[i])
    for j in AtomType:
        origin.write("%12.5f" % LOC[i][j])
    origin.write("\n")
origin.close()

PVDOS = open("pvdos.dat", "w")
DELTA = 1.0 * (plot[1] - plot[0]) / (plot[2] - 1)
PVDOS.write("%12s" % 'WAVENUMBER')
for j in AtomType:
    PVDOS.write("%12s" % j)
PVDOS.write("\n")
for wn in xrange(plot[2] + 1):
    wavenum = wn * DELTA + plot[0]
    PVDOS.write("%12.5f" % wavenum)
    loc_tmp = {i: 0 for i in AtomType}
    for i in xrange(3 * NUMBER):
        for j in AtomType:
            dx = wavenum - WAVENUM[i]
            loc_tmp[j] += LOC[i][j] * exp(-dx**2 / (2 * Gauss_C**2))
    for j in AtomType:
        PVDOS.write("%12.5f" % loc_tmp[j])
    PVDOS.write("\n")
PVDOS.close()