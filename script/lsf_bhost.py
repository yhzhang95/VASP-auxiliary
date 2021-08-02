#!/usr/bin/env python
from os import popen

Userlist = [
    "lsfadmin",
    "xiaolin",
    "weiweiwu",
    "zqxu",
    "pbsadmin",
    "txli",
    "wangrui",
    "qjxiao",
    "zhanjun",
    "jgq",
    "bupt",
    "jlwang",
    "wbdou",
    "rxiong",
    "iqubic",
    "assadmin",
    "shuai.dong",
    "weiwei.sun",
    "waiting for update......",
]

color = {
    'white': "\033[37m",
    'red': "\033[1;31m",
    'cyan': "\033[36m",
    'green': "\033[32m",
    'yellow': "\033[33m",
    'normal': "\033[0m",
}


def User(node):
    user = []
    for i in xrange(1, 29):
        if "%d*%s" % (i, node) in BJOBKEYS:
            user.append(BJOBDICT["%d*%s" % (i, node)])
    if user == ["jlwang"]: return ["cyan", "jlwang"]
    else: return ["yellow", ",".join(user)]


def fomat(line):
    return "%s%-19s%-13s%4s%7s%7s%7s%15s\033[0m" % ((color[line[9]],) + tuple(line[0:6]) + (line[10],))


# Get our group running nodes
BJOBS = popen("bjobs -u all").read().split("\n")
BJOBS = [[i[8:15].strip(), i[45:56].strip()] for i in BJOBS][1:]
for i in xrange(len(BJOBS)):
    if BJOBS[i][1] == '':
        BJOBS[i][0] = None
        continue
    if BJOBS[i][0] == '': BJOBS[i][0] = BJOBS[i - 1][0]
    for name in Userlist:
        if name.find(BJOBS[i][0]) == 0:
            BJOBS[i][0] = name
            break
BJOBDICT = {i[1]: i[0] for i in BJOBS}
BJOBKEYS = BJOBDICT.keys()

# Sort BHOSTS
BHOSTS = popen("bhosts").read().split()
BHOSTS = [[BHOSTS[i * 9 + j] for j in xrange(9)] for i in xrange(len(BHOSTS) / 9)]

HOSTS = [BHOSTS[0] + ["white", "STATE"]]
for line in BHOSTS[1:]:
    if line[1] == 'unavail' or line[1] == 'unreach': HOSTS.append(line + ["red", "Error"])
    elif line[0] == 'c02n01' or line[0] == 'c02n02': HOSTS.append(line + ["red", "Error"])
    elif line[1] == 'closed' and line[4] == '0': HOSTS.append(line + ["red", "Error"])
    elif line[1] == 'ok' and line[4] == '0': HOSTS.append(line + ["green", "waiting"])
    else: HOSTS.append(line + User(line[0]))
for line in HOSTS: print fomat(line)

HOSTS = [i for line in HOSTS for i in line]
print "\nTotal   = %2d, Error   = %2d, Available   = %2d\nRunning = %2d, Waiting = %2d, Other Group = %2d\n" % (
    len(BHOSTS) - 1, HOSTS.count("Error"), len(BHOSTS) - 1 - HOSTS.count("Error"), 
    HOSTS.count("cyan"), HOSTS.count("green"), HOSTS.count("yellow"))
