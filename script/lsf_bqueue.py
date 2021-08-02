#!/usr/bin/env python
from os import popen
from sys import argv

# Read bparams
BPARAMS = popen("bparams -a").read().split("\n")

# Para
CPU_TIME_FACTOR, RUN_TIME_FACTOR, RUN_JOB_FACTOR, FAIRSHARE_ADJUSTMENT_FACTOR = 0, 0, 0, 0
for line in BPARAMS:
    if line.find("CPU_TIME_FACTOR") == 8:
        CPU_TIME_FACTOR = float(line.split()[2])
    elif line.find("RUN_TIME_FACTOR") == 8:
        RUN_TIME_FACTOR = float(line.split()[2])
    elif line.find("RUN_JOB_FACTOR") == 8:
        RUN_JOB_FACTOR = float(line.split()[2])
    elif line.find("FAIRSHARE_ADJUSTMENT_FACTOR") == 8:
        FAIRSHARE_ADJUSTMENT_FACTOR = float(line.split()[2])

# Help
if len(argv) > 1 and argv[1] == '--help':
    print "\tCPU_TIME_FACTOR             =", CPU_TIME_FACTOR
    print "\tRUN_TIME_FACTOR             =", RUN_TIME_FACTOR
    print "\tRUN_JOB_FACTOR              =", RUN_JOB_FACTOR
    print "\tFAIRSHARE_ADJUSTMENT_FACTOR =", FAIRSHARE_ADJUSTMENT_FACTOR
    print "DYNAMIC PRIORITY = SHARES / ( CPU_TIME * CPU_TIME_FACTOR + RUN_TIME * RUN_TIME_FACTOR +"
    print "           SLOTS * RUN_JOB_FACTOR + RUN_JOB_FACTOR + ADJUST * FAIRSHARE_ADJUSTMENT_FACTOR )"
    exit(0)


# Get data
QUEUE = argv[1] if len(argv) > 1 else "normal"
BQUEUE = popen("bqueues -l %s" % QUEUE).read().split("\n")
# dynamic priority = 
#          number_shares / (
#                        cpu_time * CPU_TIME_FACTOR +
#                        run_time * RUN_TIME_FACTOR +
#                        (1 + job_slots) * RUN_JOB_FACTOR +
#                        fairshare_adjustment*FAIRSHARE_ADJUSTMENT_FACTOR
#                       )
STARTLINE = [i + 2 for i in xrange(len(BQUEUE)) if BQUEUE[i].find("SHARE_INFO_FOR") != -1][0]
ENDLINE = [STARTLINE + i for i in xrange(len(BQUEUE[STARTLINE:])) if BQUEUE[i + STARTLINE] == ''][0]


# Get string of PRIORITY
def priority(PRIORITY):
    if PRIORITY <  0.0001:
        string = "%9.4G" % PRIORITY
        return string[0:-2] + string[-1]
    else:
        return "%8.4G" % PRIORITY

# function of calculate priority
def Calc_priority(line):
    # sort data
    DATA = line.split()
    USER, SHARES, JOB_SLOT = DATA[0], int(DATA[1]), int(DATA[3])
    CPU_TIME, RUN_TIME, ADJUST = float(DATA[5]), float(DATA[6]), float(DATA[7])
    PRIORITY = SHARES / (
        CPU_TIME * CPU_TIME_FACTOR + 
        RUN_TIME * RUN_TIME_FACTOR +
        (1 + JOB_SLOT) * RUN_JOB_FACTOR + 
        ADJUST * FAIRSHARE_ADJUSTMENT_FACTOR)
    # format print
    DATA = " " * 9 + "-" * 50 + "\n"
    DATA += "%-14s%7d%6d%12.1f%10.0f%7.3G  [%s]\n" % (
        USER, SHARES, JOB_SLOT, CPU_TIME, RUN_TIME, ADJUST, priority(PRIORITY))
    DATA += "%-14s%6d*%5.f*%11.f*%9.f*%6.2G*%11.f*\n" % (
        " CONTRIBUTION", SHARES, JOB_SLOT * RUN_JOB_FACTOR,
        CPU_TIME * CPU_TIME_FACTOR, RUN_TIME * RUN_TIME_FACTOR,
        ADJUST * FAIRSHARE_ADJUSTMENT_FACTOR, SHARES / PRIORITY)
    return DATA


# print result
TITLE = "%-14s%7s%6s%12s%10s%7s%12s\n" % ("USER/GROUP", "SHARES", "SLOTS", "CPU_TIME", "RUN_TIME", "ADJUST", "PRIORITY")
TITLE += "".join([Calc_priority(line) for line in BQUEUE[STARTLINE:ENDLINE]])
TITLE += "\n* CPU_TIME = The cumulative CPU time used by the user (measured in hours).\n"
TITLE += "* RUN_TIME = The total run time of running jobs (measured in hours)."
print TITLE
