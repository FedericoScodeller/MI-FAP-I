#!/bin/bash

N=10
data="../Data"
result="result_from_function_2.txt"

for file in "$data"/*.json; do
   echo "working on " $file
    for ((i=1; i<=N; i++)); do
        ./TimedInputTest "$file" $result
    done
done
