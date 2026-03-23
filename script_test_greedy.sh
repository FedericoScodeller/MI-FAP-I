#!/bin/bash

N=1
bin=./tests/bin/TestGreedy
data="./data"
result="./results/result_greedy.txt"

for file in "$data"/*.json; do
   echo "working on " $file
   $bin "$file" "$result" "$N"
done
