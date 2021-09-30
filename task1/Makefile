CC = clang
CFLAGS = -g -O2 -fopenmp
LD = clang
LDFLAGS = -g -fopenmp

PROGRAM = main

OUT=run
all:	${PROGRAM}.out

${PROGRAM}.out: ${PROGRAM}.o
	${LD} ${LDFLAGS} $< -o ${OUT}.out
	./${OUT}.out

${PROGRAM}.o: ${PROGRAM}.c
	${CC} ${CFLAGS} -c $< -o ${PROGRAM}.o

launch:
	./${OUT}.out

clean:
	rm -f ${PROGRAM}.o ${OUT}.out
