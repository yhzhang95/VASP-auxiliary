#!/usr/bin/env python
# -*- coding: utf-8 -*-
from sys import stdout
from pprint import pprint
from math import pi


def outer_product(vector1, vector2):
    vector_tmp = [0, 0, 0]
    vector_tmp[0] = vector1[1] * vector2[2] - vector1[2] * vector2[1]
    vector_tmp[1] = vector1[2] * vector2[0] - vector1[0] * vector2[2]
    vector_tmp[2] = vector1[0] * vector2[1] - vector1[1] * vector2[0]
    return vector_tmp


def inner_product(vector1, vector2):
    sum = vector1[0] * vector2[0]
    sum += vector1[1] * vector2[1]
    sum += vector1[2] * vector2[2]
    return sum


def reciprocal_lattice():
    POSCAR = open("POSCAR", "r").readlines()
    A[0] = [float(i) for i in POSCAR[2].split()]
    A[1] = [float(i) for i in POSCAR[3].split()]
    A[2] = [float(i) for i in POSCAR[4].split()]
    B[0] = outer_product(A[1], A[2])
    B[1] = outer_product(A[2], A[0])
    B[2] = outer_product(A[0], A[1])
    OMEGA = inner_product(A[0], B[0])
    for Bi in B:
        for i in xrange(len(Bi)):
            Bi[i] = Bi[i] * 2 * pi / OMEGA

def create_KPOINTS():
	KPOINTS = open("KPOINTS", "w")
    KPOINTS.write("k-points along high symmetry lines\n%d\nReciprocal\n" % number)
    

# init kline
temple = open("KPOINTS-BAND","r").readlines()
NUM = int(temple[0])
KLINE = []
for line in temple[1:]:
    if line == "\n":
        continue
    KLINE.append([float(line.split()[i]) for i in xrange(3)])
pprint(KLINE)

# init lattice
A = [[0 for j in xrange(3)] for i in xrange(3)]
B = [[0 for j in xrange(3)] for i in xrange(3)]
reciprocal_lattice()

# pre-calculate
print len(KLINE)

# output
#band_calculate()