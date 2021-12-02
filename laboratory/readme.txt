Laboratory work Parallel Computing MIPT. my var 2б.
Build: make
All further actions please do in output directory. All scripts will be copied.

./launch.sh {program.out} - launches timing measurement for {program.out} on different process number (1..12). Output in timing.dat.
./compare.sh {my/common} - launches correctess checking for {common} or {my} variant. Compares parallel program with linear.
./job.sh - launches parallel_my.out on cluster with different process number {1..32}. Measures time.
./linear_my.out - linear program of my variant.
./linear_common.out - linear program of common variant.
./parallel_my.out [param] - parallel program of my variant. Does results printing if there is any [param].
./parallel_common.out [param] - parallel program of common variant. Does results printing if there is any [param].

results direcotry contains results of the common and my variant runs on cluster.
