
CFLAGS := -std=c99
LFLAGS := -lm
SRC := src
OUT := output

all : ${OUT} linear_common linear_my parallel_common parallel_my

${OUT} :
	@mkdir -p ${OUT}
	@cp launch.sh plot.py ${OUT}

linear_common : 
	-gcc ${CFLAGS} ${SRC}/$@.c -o ${OUT}/$@.out ${LFLAGS}

linear_my :
	-gcc ${CFLAGS} ${SRC}/$@.c -o ${OUT}/$@.out ${LFLAGS}

parallel_common :
	-mpicc ${CFLAGS} ${SRC}/$@.c -o ${OUT}/$@.out ${LFLAGS}

parallel_my :
	-mpicc ${CFLAGS} ${SRC}/$@.c -o ${OUT}/$@.out ${LFLAGS}


clean :
	rm -rf ${OUT}