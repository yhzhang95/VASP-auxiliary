#!/usr/bin/env python
from os import popen, system
from time import sleep
from sys import argv, stdout


def bjob():
    # Get lsload of HOSTS
    nodes = [line.split() for line in popen("lsload").read().split("\n")[1:-1]]
    LOAD = {line[0]: line[5] if line[1] == 'ok' else 'Err' for line in nodes}
    MEMO = {line[0]: "%5G" % float(line[6]) if line[1] == 'ok' else '  Err' for line in nodes}

    # Get our group running nodes
    BJOBS = popen("bjobs -u all").read().split("\n")
    BJOBS = [line[0:16] + line[22:29] + line[44:56] + line[57:] for line in BJOBS]

    # Get our group running nodes detail
    BJOBSL = popen("bjobs -l -u all").read().split("\n")
    BJOBSORT, tmp, FLAG = [], None, False
    for line in BJOBSL:
        if line == '': continue
        if line[:3] == 'Job': FLAG = True
        elif line[:11] == ' SCHEDULING': FLAG = False
        if FLAG == False: continue

        if line[0] != ' ':
            BJOBSORT.append(tmp)
            tmp = line
        elif line.find(" "*21) == 0: tmp += line[21:]
    BJOBSORT.append(tmp)

    BJOBSSORT, EXECTIME, SLOTS, FLAG = {}, 0, 0, 0
    for line in BJOBSORT[1:] + ["Job <29>,"]:
        if line[:3] == "Job":
            if FLAG == 2: BJOBSSORT[JOBID] = " " * 13 + "%4s" % SLOTS
            elif FLAG in [3, 4]: BJOBSSORT[JOBID] = EXECTIME + " " + "%4s" % SLOTS
            FLAG = 0
        if FLAG == 0: JOBID = int(line.split()[1].strip("<>,"))
        elif FLAG == 1:
            SLOTS = [l.split()[0] for l in line.split(",") if l.find("Task(s)") != -1]
            if len(SLOTS) > 0: SLOTS = SLOTS[0]
            else: SLOTS = "Err"
        elif FLAG == 2: EXECTIME = line[4:16]
        FLAG += 1
    
    BJOBS[0] += "  EXECUTE_TIME SLOT  CPU MEMO"
    for i, line in enumerate(BJOBS[1:-1]):
        if line[0] == " ":
            _host = line[24:35].strip()
            host = _host.split("*")[1] if "*" in _host else _host
            BJOBS[i + 1] += " " * 44 + "%4s" % LOAD[host] + MEMO[host]
        else:
            BJOBS[i + 1] += " " + BJOBSSORT[int(line[:6])]
            if line[24:35].strip() != '':
                _host = line[24:35].strip()
                host = _host.split("*")[1] if "*" in _host else _host
                BJOBS[i + 1] += " " + "%4s" % LOAD[host] + MEMO[host]
            

    for line in BJOBS:
        stdout.write(line + "\n")

if len(argv) > 1:
    system("bjobs -l %s" % argv[1])
    exit(0)

DEBUG = False
if DEBUG:
    bjob()
else:
    while True:
        try:
            bjob()
            break
        except:
            sleep(0.1)
