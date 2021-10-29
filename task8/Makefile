CC = clang
CFLAGS = -g -O2 -fopenmp
LD = clang
LDFLAGS = -g -fopenmp

PROGRAM = main
GENERATOR = generator

OUT=run
GEN_OUT=gen
all:	${PROGRAM}.out ${GENERATOR}.out

${PROGRAM}.out: ${PROGRAM}.o
	${LD} ${LDFLAGS} $< -o ${OUT}.out

${PROGRAM}.o: ${PROGRAM}.c
	${CC} ${CFLAGS} -c $< -o ${PROGRAM}.o

${GENERATOR}.out: ${GENERATOR}.o
	${LD} ${LDFLAGS} $< -o ${GEN_OUT}.out

${GENERATOR}.o: ${GENERATOR}.c
	${CC} ${CFLAGS} -c $< -o ${GENERATOR}.o

launch:
	./${GEN_OUT}.out 10000 out.txt
	./${OUT}.out out.txt

clean:
	rm -f *.o *.out
