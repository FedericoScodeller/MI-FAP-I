#!/bin/bash

N=10
bin=./tests/bin/TestInputTime
data="./data"
result="./results/test_input_ver2.4.txt"

for file in "$data"/*.json; do
   echo "working on " $file
    for ((i=1; i<=N; i++)); do
         $bin "$file" "$result"
    done
done
