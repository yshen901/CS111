#!/bin/bash

./lab4b --period=2 --scale=C --log=test.txt 2> err.txt <<-EOF
START
LOG HELLO WORLD
PERIOD=5
SCALE=F
STOP
OFF
EOF


if [ $? -ne 0 ]
then
    echo "ERROR program has invalid exit code"
else
    echo "OK passed smoketest"
fi
