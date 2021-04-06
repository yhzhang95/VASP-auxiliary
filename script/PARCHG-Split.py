from numpy import array, arange

# Pre setting
FileName = "PARCHG01630001"
SplitPara = 0.5
CHG = array(open(FileName, "r").readlines())

# Parameters
SYSTEM = CHG[0].rstrip()
LATTICE = array([CHG[i].split() for i in (2, 3, 4)], dtype='float64')
WEIGHT = array(CHG[6].split(), dtype='int32')
ATOM = array([
    CHG[5].split()[i] for i in xrange(len(WEIGHT)) for j in xrange(WEIGHT[i])
])
NUMBER = WEIGHT.sum()
UP = array([float(CHG[i].split()[2])
            for i in xrange(8, 8 + NUMBER)]) > SplitPara
DOWN = UP == False

# Get grid data
NGX, NGY, NGZ = array(CHG[9 + NUMBER].split(), dtype='int32')
CHGDAT = array(
    [line.split() for line in CHG[10 + NUMBER:]], dtype='float64').reshape(
        NGZ, NGY, NGX)

# Output UP POSCAR-CHG
DATA = array(range(8) + arange(8, 8 + NUMBER)[UP].tolist() + [8 + NUMBER, 9 + NUMBER])
ATOMNEW = ATOM[UP]
KEYS = []
[KEYS.append(key) for key in ATOMNEW if key not in KEYS]
VALUES = [list(ATOMNEW).count(key) for key in KEYS]
CHG[5] = "".join(["%5s" % i for i in KEYS]) + "\n"
CHG[6] = "".join(["%6d" % i for i in VALUES]) + "\n"
UPFILE = open(FileName + "UP", "w")
UPFILE.writelines(CHG[DATA])
flag = 0
for Z in xrange(NGZ):
    for Y in xrange(NGY):
        for X in xrange(NGX):
            if flag == 10:
                flag = 0
                UPFILE.write("\n")
            if float(Z) > NGZ * SplitPara:
                UPFILE.write("%+7.5E " % CHGDAT[Z, Y, X])
            else:
                UPFILE.write("%+7.5E " % 0)
            flag += 1
UPFILE.close()

# Output DOWN POSCAR-CHG
DATA = array(range(8) + arange(8, 8 + NUMBER)[DOWN].tolist() + [8 + NUMBER, 9 + NUMBER])
ATOMNEW = ATOM[DOWN]
KEYS = []
[KEYS.append(key) for key in ATOMNEW if key not in KEYS]
VALUES = [list(ATOMNEW).count(key) for key in KEYS]
CHG[5] = "".join(["%5s" % i for i in KEYS]) + "\n"
CHG[6] = "".join(["%6d" % i for i in VALUES]) + "\n"
DOWNFILE = open(FileName + "DOWN", "w")
DOWNFILE.writelines(CHG[DATA])
flag = 0
for Z in xrange(NGZ):
    for Y in xrange(NGY):
        for X in xrange(NGX):
            if flag == 10:
                flag = 0
                DOWNFILE.write("\n")
            if float(Z) <= NGZ * SplitPara:
                DOWNFILE.write("%+7.5E " % CHGDAT[Z, Y, X])
            else:
                DOWNFILE.write("%+7.5E " % 0)
            flag += 1
DOWNFILE.close()