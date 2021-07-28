#!/usr/bin/env python
try:
    from xml.etree import cElementTree as et
except:
    from xml.etree import ElementTree as et
import argparse
import sys
import re
import os


def get_parser():
    parser = argparse.ArgumentParser(
        description='Pretty list OpenPBS/Torque Workload Manager.')

    parser.add_argument('--cutoff',
                        dest='cutoff',
                        action='store_true',
                        help='default')
    parser.add_argument('--no-cutoff',
                        dest='cutoff',
                        action='store_false',
                        help='')
    parser.set_defaults(cutoff=True)

    parser.add_argument('--header',
                        dest='header',
                        action='store_true',
                        help='default')
    parser.add_argument('--no-header',
                        dest='header',
                        action='store_false',
                        help='')
    parser.set_defaults(header=True)

    parser.add_argument('--time',
                        dest='time',
                        action='store_true',
                        help='default')
    parser.add_argument('--no-time',
                        dest='time',
                        action='store_false',
                        help='')
    parser.set_defaults(time=True)

    parser.add_argument('--njobname',
                        default=8,
                        type=int,
                        help='The length of jobname.')
    parser.add_argument('--maxcols',
                        default=int(os.popen('tput cols').read()),
                        type=int,
                        help='The max length of windows cols.')

    parser.add_argument('--disable',
                        default="",
                        type=str,
                        help='Ignore the queue which is not interested.')
    parser.add_argument('--enable',
                        default="",
                        type=str,
                        help='Enable the queue which is interested.')

    args = parser.parse_args()

    args.disable = args.disable.split()
    args.enable = args.enable.split()

    return args


class Format(object):
    def __init__(self, args):
        self.args = args
        self.header = ''
        self.separater = ''

    def set_length(self, jobinfos):
        # get maxlength of each element
        self.jobid = max([len(info.jobid) for info in jobinfos]) + 1
        self.user = max([len(info.user) for info in jobinfos])
        self.qlist = max([len(info.qlist) for info in jobinfos])
        self.name = self.args.njobname
        self.node = max(
            [len(node) for info in jobinfos for node in info.nodes])
        self.state = 1
        self.nodect = 1
        self.workcol = self.args.maxcols - self.jobid - self.user -  \
            self.qlist - self.name - self.node - self.state - self.nodect - 8

        # set format string for later work
        self.fmt = '{:>%d} {:>%d} {:>%d} {:>%d} {:>%d} {:>%d} {:>%d} {:<%d}\n' % (
            self.jobid, self.user, self.qlist, self.name, self.node,
            self.state, self.nodect, int(self.workcol / 2))
        self.fmt_sep = self.fmt.replace('>', '-^').replace('<', '-^')

        # set header and separater
        self.header = self.fmt.format(
            'Jobid', 'User', 'Walltime' if self.args.time else 'Queue',
            'Jobname', 'Nodes', 'S', 'N', 'Workdir')
        self.separater = self.fmt_sep.format('', '', '', '', '', '', '', '')


class JobInfo(object):
    def __init__(self, child, prefixs):
        self.jobid = child.find('Job_Id').text
        self.name = child.find('Job_Name').text
        self.queue = child.find('queue').text
        self.user = child.find('Job_Owner').text.split('@')[0]
        self.id = int(self.jobid.split('.')[0])

        self.nodect = int(child.find('Resource_List').find('nodect').text)
        self.state = child.find('job_state').text
        self.nodes = self.get_nodes(child)
        self.workdir = self.get_workdir(child, prefixs)

        self.qlist = ''
        if child.find('resources_used') is not None:
            self.qlist = child.find('resources_used').find('walltime').text

    def get_string(self, fmt, args):
        if len(self.name) > fmt.name:
            self.name = self.name[:fmt.name - 1] + '.'

        if args.cutoff and (len(self.workdir) > fmt.workcol):
            self.workdir = self.workdir[:fmt.workcol - 2] + '..'

        if args.time is False:
            self.qlist = self.queue

        if sys.version_info[0] == 2:
            line = fmt.fmt.encode('utf-8').format(
                self.jobid,
                self.user,
                self.qlist,
                self.name.encode('utf-8'),
                self.nodes[0],
                self.state,
                self.nodect,
                self.workdir.encode('utf-8'),
            )
        elif sys.version_info[0] == 3:
            line = fmt.fmt.format(
                self.jobid,
                self.user,
                self.qlist,
                self.name,
                self.nodes[0],
                self.state,
                self.nodect,
                self.workdir,
            )

        collects = [[self.queue, self.id, 0, line]]
        for idx, node in enumerate(self.nodes[1:], 1):
            line = fmt.fmt.format('', '', '', '', node, '', '', '')
            collects.append([self.queue, self.id, idx, line])

        return collects

    def get_nodes(self, child):
        if self.state != 'R':
            nodes = ['']
        else:
            nodes = child.find('exec_host').text
            nodes = [i.split('/')[0] for i in nodes.split('+')]
            nodes = [
                '%d*%s' % (nodes.count(nd), nd)
                for nd in sorted(set(nodes), key=nodes.index)
            ]

        return nodes

    def get_workdir(self, child, prefixs):
        variable = child.find('Output_Path').text
        lidx, ridx = variable.find(':'), variable.rfind('/')
        workdir = variable[lidx + 1:ridx]
        workdir = re.sub('^' + prefixs[self.user], '~', workdir)

        return workdir


def get_queueinfo():
    queuedict = {}
    queueinfo = os.popen('qstat -Q').read().split('\n')[:-1]
    fmts = '{:s}: {:15s} {:s}: {:s} {:s}: {:s} {:s}: {:s} {:s}: {:s} {:s}: {:s}\n'

    # example:
    #   Queue              Max    Tot   Ena   Str   Que   Run   Hld   Wat   Trn   Ext T   Cpt
    #   ----------------   ---   ----    --    --   ---   ---   ---   ---   ---   --- -   ---
    #   zhou                 0      0   yes   yes     0     0     0     0     0     0 E     0
    #   gold6240R            0     10   yes   yes     4     6     0     0     0     0 E     0
    header = queueinfo[0].split()
    for line in queueinfo[2:]:
        temp = line.split()
        # fmts.format(Queue, gold6240R, Tot, 10, Que, 4, Run, 6, Hld, 0, Wat, 0)
        strings = fmts.format(header[0], temp[0], header[2], temp[2],
                              header[5], temp[5], header[6], temp[6],
                              header[7], temp[7], header[8], temp[8])
        # queuedict['gold6240R'] = 'Queue: gold6240R       Tot: 10 Que: 4 Run: 6 Hld: 0 Wat: 0'
        queuedict[temp[0]] = strings

    return queuedict


def display(fmt, args, collects, queuedict={}):
    # collects: [queue, id, index, string]
    queues = list(set([queue for queue, _, _, _ in collects]))
    queues = sorted(queues)
    if args.enable:
        queues = [queue for queue in queues if queue in args.enable]
    else:
        queues = [queue for queue in queues if queue not in args.disable]

    collects = sorted(collects)
    for idx, queue in enumerate(queues):
        if idx != 0: sys.stdout.write('\n')

        sys.stdout.write(queuedict.get(queue, ''))
        sys.stdout.write(fmt.header)
        sys.stdout.write(fmt.separater)
        for _queue, _id, _index, _string in collects:
            if _queue == queue:
                sys.stdout.write(_string)


def main():
    # get args
    args = get_parser()
    fmt = Format(args)

    # pre-definition
    # example:
    #       root:x:0:0:root:/root:/bin/bash
    #       bin:x:1:1:bin:/bin:/sbin/nologin
    # prefixs ==> dict(user: $HOME)
    prefixs = open('/etc/passwd', 'r').readlines()
    prefixs = {line.split(':')[0]: line.split(':')[5] for line in prefixs}

    # get queue information
    queuedict = get_queueinfo() if args.header else {}

    # main loop
    try:
        root = et.fromstring(os.popen('qstat -x').read())
    except Exception as e:
        sys.stderr.write(e + '\n')
        exit(1)
    jobinfos = [JobInfo(child, prefixs) for child in root]

    fmt.set_length(jobinfos)
    collects = [clt for info in jobinfos for clt in info.get_string(fmt, args)]
    display(fmt, args, collects, queuedict)


if __name__ == "__main__":
    main()
