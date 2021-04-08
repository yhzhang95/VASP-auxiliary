#!/usr/bin/env python
from math import sqrt, acos, degrees
from sys import argv

# format pre-define
header = '''#======================================================================
# CRYSTAL DATA
#----------------------------------------------------------------------
data_convert_by_YehuiZhang

_chemical_name_common                  '%s'
_cell_length_a                         %.5F
_cell_length_b                         %.5F
_cell_length_c                         %.5F
_cell_angle_alpha                      %.4F
_cell_angle_beta                       %.4F
_cell_angle_gamma                      %.4F
_space_group_name_H-M_alt              'P 1'
_space_group_IT_number                 1

loop_
_space_group_symop_operation_xyz
   'x, y, z'

loop_
   _atom_site_label
   _atom_site_occupancy
   _atom_site_fract_x
   _atom_site_fract_y
   _atom_site_fract_z
   _atom_site_adp_type
   _atom_site_B_iso_or_equiv
   _atom_site_type_symbol
'''
formats = '   %-8s   1.0 %12.8F %12.8F %12.8F  Biso  1.000 %s\n'

# Read POSCAR or CONTCAR
filename = argv[1] if len(argv) > 1 else 'POSCAR'
vasp = open(filename, 'r').readlines()

title = vasp[0].rstrip('\n')
lat = [[float(i) for i in line.split()] for line in vasp[2:5]]
atom_type, atom_num = vasp[5].split(), [int(i) for i in vasp[6].split()]
atoms = [(t, i + 1) for t, n in zip(atom_type, atom_num) for i in range(n)]
pos = [[float(i) for i in line.split()] for line in vasp[8:8 + sum(atom_num)]]

# Calculate parameters
lat_norm = [sqrt(sum([i**2 for i in axis])) for axis in lat]
lat_dot = [
    sum([lat[i][k] * lat[j][k] for k in range(3)])
    for i, j in [(0, 1), (1, 2), (2, 0)]
]

(a, b, c), (ab, bc, ac) = lat_norm, lat_dot
ang = lambda a, b, ab: degrees(acos(ab / (a * b)))
alpha, beta, gamma = ang(b, c, bc), ang(a, c, ac), ang(a, b, ab)

# Output *.cif
cifstr = header % (title, a, b, c, alpha, beta, gamma)
for idx, ((_type, idx), (x, y, z)) in enumerate(zip(atoms, pos)):
    cifstr += formats % (_type + str(idx), x, y, z, _type)
open(filename + '.cif', 'w').writelines(cifstr)
