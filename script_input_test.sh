#!/bin/bash

N=500
bin=./tests/bin/TestGreedy
data="./data"
result="./results/test_solver_greedy.txt"

for file in "$data"/*.json; do
   echo "working on " $file
   $bin "$file" "$result" "$N"
done
