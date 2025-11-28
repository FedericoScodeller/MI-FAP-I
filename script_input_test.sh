#!/bin/bash

N=10
data="./data"
result="test_new_input.txt"

for file in "$data"/*.json; do
   echo "working on " $file
    for ((i=1; i<=N; i++)); do
        ./tests/bin/TestInputTime "$file" $result
    done
done
