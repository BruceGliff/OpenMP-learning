#!/bin/bash

#PBS -l walltime=00:01:00,nodes=4:ppn=4
#PBS -N result
#PBS -q batch

cd $PBS_O_WORKDIR

for ((a = 1; a < 33; a++))
do
        start=`date +%s.%N`
        mpirun --hostfile $PBS_NODEFILE -np $a ./parallel_my.out
        end=`date +%s.%N`
        runtime=$(awk '{print $1-$2}' <<<"$end $start")
        echo $a $runtime 
done
