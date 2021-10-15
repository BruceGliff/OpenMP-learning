#!/bin/bash
rm -rf out.txt

for i in {1..51..2}
do
  echo "Threads: $i" >> out.txt
  env OMP_NUM_THREADS=$i ./run.out 1 | grep Time >> out.txt
  if [ $? -ne 0 ]; then
    echo " Error. threads: $i" 1>&2
    exit
  fi
done

echo "Passed"
