#!/bin/bash

# launch up to 12 processes.

if [[ -z $1 ]]; then
  echo "Usage: ./launch {program.out}"
  exit 1
fi

rm timing.dat

for ((a = 1; a < 13; a++))
do
	echo "Processes $a"
	
	start=`date +%s.%N`
	mpirun -np $a ./$1
	end=`date +%s.%N`
	runtime=$(awk '{print $1-$2}' <<<"$end $start")
	echo $a $runtime >> timing.dat
done


echo "Done."
