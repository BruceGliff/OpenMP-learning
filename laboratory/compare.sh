#!/bin/bash

# launch up to 12 threads.

if [[ -z $1 ]]; then
  echo "Usage: ./compare {my or common}"
  exit 1
fi

./linear_$1.out

for a in {1..13..3}; do
  mpirun -np $a ./parallel_$1.out 1
  diff -q result_linear_$1.dat result_parallel_$1.dat
  if [[ $? -ne 0 ]]; then
    echo "Diff with $a threads."
    exit 1
  fi
done

echo "linear_$1 and parallel_$1 are identical."
