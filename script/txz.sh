#!/bin/bash

# Parse passing argument.
[ -n "$1" ] && FOLDER=${1%/} || FOLDER="None"
[ -n "$2" ] && COMPRESS=$2 || COMPRESS=6

# Check if the folder exists
[ -d ${FOLDER} ] && HEAD='Find' || HEAD="Can't find"
echo "${HEAD} folder name: ${FOLDER}"
! [ -d ${FOLDER} ] && exit 1

# Execute compression command.
xz_opts="-${COMPRESS} -T4 -cv"
excepts="--exclude=WAVECAR --exclude=CHG --exclude=CHGCAR"
echo "Compress folder except (WAVECAR, CHG*) by xz -${COMPRESS}."
tar -cf - ${FOLDER} ${excepts} | xz ${xz_opts} - > ${FOLDER}.tar.xz

# Touch file when xz finish
touch ${FOLDER}.tar.xz.done