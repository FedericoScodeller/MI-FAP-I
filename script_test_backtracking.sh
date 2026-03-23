#!/bin/bash

bin=./tests/bin/TestBacktracking
data="./data"
result="./results/Backtracking/ver_2.1/"

for file in "$data"/*.json; do
   echo "working on " $file
   out=${file/$data}
   $bin $file >> $result${out/json/txt} 2>&1
done
