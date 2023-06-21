#!/bin/bash
INPUTFILES=(inputfiles/inputfile*)
OUTPUTFILES=(outputfiles/outputfile*)

for ((i = 0; i < ${#INPUTFILES[@]}; i++)); do
    echo "Test $i	: ./a.out < inputfiles/inputfile$i | diff -B -w - outputfiles/outputfile$i"
    RESULT=$(./a.out < "${INPUTFILES[$i]}" | diff -B -w - "${OUTPUTFILES[$i]}")
    if [[ $RESULT == "" ]]; then
        echo "TEST PASSED!"
    else
        echo "$RESULT"
    fi
done
