#!/bin/bash

# launch up to 12 threads.

if [[ -z $1 ]]; then
  echo "Usage: ./launch {program.out}"
  exit 1
fi

rm timing.dat

for ((a = 1; a < 13; a++))
do
	echo "Threads $a"
	/usr/bin/time -f "%e %S" -o .tmp.timing mpirun -np $a ./$1
	echo $a "$(< .tmp.timing)" >> timing.dat
done

rm .tmp.timing



echo "Done."
