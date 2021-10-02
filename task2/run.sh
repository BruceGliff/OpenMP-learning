#!/bin/bash

rm -rf out.txt

for i in {1..101..2}
do
  echo "Threads: $i" >> out.txt
  for j in {1..100000..501}
  do
    echo "    N: $j" >> out.txt
    env OMP_NUM_THREADS=$i ./run.out $j 1 >> out.txt
    if [ $? -ne 0 ]; then
      echo " Error. threads: $i, N: $j" 1>&2
      exit
    fi
  done
done
