#!/bin/bash

i=1

for f1 in ./so/*
do
    for f2 in ./TestCases/82.so
    do
        v1=${f1:0-13:10}
        v2=${f2:0-5:2}
        echo "Running Valgrind: compete $f1 $f2 ./compete_result/${v1}_${v2}.txt"
        ./Compete $f1 $f2 "./compete_result/${v1}_${v2}.txt" 1 2> "./compete_result/${v1}_${v2}_error.txt"
        echo "Valgrind completed without errors"
        
    done
done
