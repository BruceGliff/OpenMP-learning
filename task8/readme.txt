Task with "*"!
Sorting an array.
Build: make
Launch: ./run.out [in file]
        ./gen.out SIZE [out_file]
Script: ./run.sh launches sorting of the random generated array with 10000 size.

Warn:
0. Makefile builds both sorting and generating programs.
1. In case of using console input, please consider using Ctrl+D to interupt entering.
2. It default prints to console.

Possible usage:
$ ./run.out
3 2 1
0 4 548 65
Ctrl+D

$ ./run.out in.txt

$ ./gen.out 42 out.txt
$ ./run.out out.txt

$ ./gen.out 42 | ./run.out