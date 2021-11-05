CC = clang
CFLAGS = -g -O2 -fopenmp
LD = clang
LDFLAGS = -g -fopenmp -lm

PROGRAM = main

OUT=run
all:	${PROGRAM}.out

${PROGRAM}.out: ${PROGRAM}.o
	${LD} ${LDFLAGS} $< -o ${OUT}.out

${PROGRAM}.o: ${PROGRAM}.c
	${CC} ${CFLAGS} -c $< -o ${PROGRAM}.o

launch:
	./${OUT}.out 10000 1

clean:
	rm -f *.o *.out
