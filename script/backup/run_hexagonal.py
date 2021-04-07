#!/usr/bin/env python
# -*- coding: utf-8 -*-
from sys import stdout
from time import time
from math import sqrt
from os import system
from sys import argv

start = argv[1]
end = argv[2]
delta = argv[3]
NP = argv[4]


def print_time(total_time):
    hour = int(total_time / 3600)
    minute = int((total_time - 3600 * hour) / 60)
    second = total_time - 3600 * hour - 60 * minute
    print "(%2d[h] %2d[m] %5.2f[s])" % (hour, minute, second)


def grep(LatticeParameter):
    infile = open("OUTCAR.%f" % LatticeParameter, "r").readlines()
    for line in infile[::-1]:
        if "free  energy   TOTEN" in line:
            print "Energy: ", line.split()[5]
            break


def run_hexagonal(LatticeParameter, NP):
    a21 = LatticeParameter * -0.5
    a22 = LatticeParameter * sqrt(3) / 2
    system("m4 -Da11=%f -Da21=%f -Da22=%f POSCAR.m4 > POSCAR" %
           (LatticeParameter, a21, a22))
    system("mpirun -np %d vasp.std.5.4.1 > log.%f" % (NP, LatticeParameter))
    system("mv OUTCAR OUTCAR.%f" % LatticeParameter)
    system("mv CONTCAR CONTCAR.%f" % LatticeParameter)


for number in xrange(int((end - start) / delta) + 1):
    # First print pre-word
    LatticeParameter = start + delta * number
    start_time = time()
    print "VASP_relax: LatticeParameter(a) = %8.5f ..." % LatticeParameter,
    stdout.flush()
    # run vasp jobs
    run_hexagonal(LatticeParameter, NP)
    # Final print cal-time
    print " Done!",
    print_time(time() - start_time)
    grep(LatticeParameter)
    stdout.flush()
