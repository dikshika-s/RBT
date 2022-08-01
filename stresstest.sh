#! /usr/bin/env bash

if [ -f './RBSTsimple' ]; then
    inp1=""; inp2=""
    echo "Preparing input... "$(date +"%T.%6N")"    This may take a while"
    echo "To see the output, ensure that this script was sourced"
    echo "Also ensure that the flag MINIMAL_OUTPUT was defined before compiling"
    for i in $(seq 1 10000); do inp1="${inp1}2 $(($RANDOM % 7501 - 2000))\n"; done
    for i in $(seq 1 30000); do inp2="${inp2}3 $(($RANDOM % 7501 - 2000))\n"; done
    start=$(date +'%T.%6N')
    echo "Starting insertion & deletion of elements... $start"
    echo "${inp1}1\n7\n${inp2}1\n7\n0\n" | ./RBSTsimple
    echo "Began  $start\nEnded  "$(date +"%T.%6N")
else
    echo "Please compile ./test.cpp to ./RBSTsimple and then run this script"
fi