from numpy import array

# Pre setting
POS = open("POSCAR", "r").readlines()
BAND = open("occupancy.dat", "r").readlines()
SplitPara = 0.5

# Parameters
SYSTEM = POS[0].rstrip()
LATTICE = array([POS[i].split() for i in (2, 3, 4)], dtype='float64')
ATOM = array(POS[6].split(), dtype='int32').sum()
UP = array([float(POS[i].split()[2]) for i in xrange(8, 8 + ATOM)]) > SplitPara
DOWN = UP == False

# Reading occupancy.dat
BAND[0] = "coordinate energy occupancy up down all prob\n"
for i in xrange(1, len(BAND) - 1):
    if BAND[i] == "\n":
        continue
    ORIGIN = array(BAND[i].split(), dtype='float64')
    UPFAC = ORIGIN[3:][UP].sum(keepdims=True)
    DOWNFAC = ORIGIN[3:][DOWN].sum(keepdims=True)
    ALLFAC = UPFAC + DOWNFAC
    PROBI = UPFAC / ALLFAC
    BAND[i] = "%f %f %f %f %f %f %f\n" % (ORIGIN[0], ORIGIN[1], ORIGIN[2],
                                          UPFAC, DOWNFAC, ALLFAC, PROBI)

open("occupancy_convert.dat", "w").writelines(BAND)
