#!/bin/bash

i=1

for f1 in ./so/*
do
    for f2 in ./TestCases/82.so
    do
        v1=${f1:0-13:10}
        v2=${f2:0-5:2}
        echo "Running Valgrind: compete $f1 $f2 ./compete_result/${v1}_${v2}.txt"
        valgrind --leak-check=full --error-exitcode=1 ./Compete $f1 $f2 "./compete_result/${v1}_${v2}.txt" 1 2> "./compete_result/${v1}_${v2}_error.txt"
        valgrind_exit_code=$?
        
        if [ $valgrind_exit_code -eq 0 ]; then
            echo "Valgrind completed without errors"
        else
            echo "Valgrind detected memory errors. Check the error log: ./compete_result/${v1}_${v2}_error.txt"
        fi
    done
done
