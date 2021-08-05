#!/usr/bin/env python
# mail: yhzhang95@seu.edu.cn
# author: yehui zhang / southeast university
# todo:
try:
    from xml.etree import cElementTree as ET
except:
    from xml.etree import ElementTree as ET
from sys import stdout
from math import sqrt
from glob import glob
import argparse
import re


def parse_args():
    # define parameterd
    parser = argparse.ArgumentParser('agforce.py')
    parser.add_argument('--path', type=str, default='./vasprun.xml')
    return parser.parse_args()


def parse_ediffg(fb):
    for line in fb:
        if 'EDIFFG' in line:
            EDIFFG = re.search('<i.*>(.*?)</i>', line).group(1)
            return float(EDIFFG)


def parse_selective(fb):
    for line in fb:
        readlines = []
        if 'name="positions"' in line:
            readlines.append(line)
            for line in fb:
                readlines.append(line)
                if '</varray>' in line: break

            root = ET.fromstring(''.join(readlines))
            positions = [True for _ in root]
            break

    for line in fb:
        readlines = []
        if 'name="selective"' in line:
            readlines.append(line)
            for line in fb:
                readlines.append(line)
                if '</varray>' in line: break

            root = ET.fromstring(''.join(readlines))
            positions = ['F' not in child.text for child in root]
            break

        elif '</structure>' in line:
            break

    return positions


def parse_calculation(fb, iteration, energybakup, content, positions):
    # save calculation part
    readlines = [' <calculation>\n']
    for line in fb:
        readlines.append(line)
        if '</calculation>' in line:
            break
    else:
        scstep = sum(1 for line in readlines if '</scstep>' in line)
        scstep = '{:d}*'.format(scstep)

        time = 0
        for line in readlines:
            if 'total' in line:
                # from vasp.5.4.4 src/xml.F
                #   WRITE(uixml,'(2F8.2)',ADVANCE="No") time1,time2
                #   WRITE(uixml, '("</time>")')
                text = re.search('<time.*>(.*?)</time>', line).group(1)[-8:]

                if '*' not in text:
                    time += float(text)
                else:
                    time = '***'
                    break
        else:
            time = '{:>10.1F}'.format(time)

        content += '{:>9d} {:>6s} {:>15s} {:>13s} {:>10s} {:>10s}\n'.format(
            iteration, scstep, '-', '-', '-', time)

        stdout.write(content)
        stdout.flush()
        return iteration + 1, energybakup, ''

    # xml format parsed by elementtree
    root = ET.fromstring(''.join(readlines))
    scstep = sum([child.tag == 'scstep' for child in root])

    for child in root:

        if child.get('name') == 'forces':
            maxforce = 0
            for fline, flag in zip(child, positions):
                if flag is not True: continue
                force = [float(i) for i in fline.text.split()]
                force = sqrt(sum(f**2 for f in force))
                if force > maxforce: maxforce = force

        elif child.tag == 'energy':
            for eline in child:
                if eline.get('name') == 'e_wo_entrp':
                    energy = float(eline.text)
                    if energybakup:
                        de = '{:10.8F}'.format(energy - energybakup)
                    else:
                        de = '-'

        elif child.tag == 'time':
            # time = float(child.text.split()[-1])
            time = child.text[-8:]
            if '*' not in time:
                time = '{:>10.1F}'.format(float(time))
            else:
                time = '***'

    content += '{:>9d} {:>6s} {:>15.8F} {:>13s} {:>10.6F} {:>10s}\n'.format(
        iteration, str(scstep), energy, de, maxforce, time)

    stdout.write(content)
    stdout.flush()
    return iteration + 1, energy, ''


def parse_vasprunxml(filename):
    with open(filename, 'r') as fb:
        # get ediffg value
        for line in fb:
            if '<separator name="ionic" >' in line:
                EDIFFG = parse_ediffg(fb)
                break

        # get selective information
        for line in fb:
            if '<structure name="initialpos" >' in line:
                positions = parse_selective(fb)
                break

        # setup title format
        content = '{:>9s} {:>6s} {:>15s} {:>13s} {:>10s} {:>10s}\n'.format(
            'Iteration', 'scstep', 'Energy', 'dE', 'MaxForce', 'Time(s)')

        iteration, energybakup = 1, None
        for line in fb:
            if '<calculation>' in line:
                iteration, energybakup, content = parse_calculation(
                    fb, iteration, energybakup, content, positions)

        # append information of ediffg
        if EDIFFG > 0:
            content = '* EDIFFG(change in energy less than) = '
            content += '{:G}\n'.format(EDIFFG)
        else:
            content = '* EDIFFG(all forces less than) = '
            content += '{:G}\n'.format(-EDIFFG)
        stdout.write(content)
        stdout.flush()


if __name__ == '__main__':
    args = parse_args()

    try:
        parse_vasprunxml(args.path)
    except Exception as e:
        print("type error: " + str(e))
