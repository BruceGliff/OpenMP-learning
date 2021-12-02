#include <math.h>
#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define ISIZE 1000
#define JSIZE 1000

typedef struct _matrix {
  double *data;
  unsigned height;
  unsigned width;
} matrix;

double *getRow(matrix m, unsigned i) { return m.data + i * m.width; }
double getVal(matrix m, unsigned i, unsigned j) { return getRow(m, i)[j]; }
void setVal(matrix m, unsigned i, unsigned j, double val) {
  getRow(m, i)[j] = val;
}

matrix createMatrix(int height, int width) {
  double *data = (double *)malloc(height * width * sizeof(double));
  matrix tmp = {data, height, width};
  return tmp;
}

void releaseMatrix(matrix m) { free(m.data); }

int main(int argc, char **argv) {

  MPI_Init(NULL, NULL);

  int th_num = 1;
  int rank = 0;

  MPI_Comm_size(MPI_COMM_WORLD, &th_num);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);

  // There is the separation on th_num tasks.
  int const chunk_tmp = ISIZE / th_num;
  int const chunk_overhead = ISIZE % th_num;
  int const th_count = ISIZE > th_num ? th_num : ISIZE;
  int const inc = chunk_overhead - rank > 0 ? 1 : 0;
  int const chunk_sz = chunk_tmp + inc;
  int const begin = inc ? rank * chunk_sz : rank * chunk_sz + chunk_overhead;
  int const end = begin + chunk_sz - 1;

  matrix a;
  if (!rank)
    a = createMatrix(ISIZE, JSIZE);
  else
    a = createMatrix(chunk_sz, JSIZE);

  int i, j;
  for (i = 0; i != chunk_sz; ++i) // Not a.height!
    for (j = 0; j != a.width; ++j)
      setVal(a, i, j, 10 * (i + begin) + j);

  for (i = 0; i != chunk_sz; ++i) // Not a.height!
    for (j = 0; j != a.width; ++j)
      setVal(a, i, j, sin(0.00001 * getVal(a, i, j)));

  // Main thread do results collecting and printing.
  if (!rank) {
    int expected = 0;
    while (++expected != th_count) {
      int const chunk_overhead = ISIZE % th_num;
      int const e_inc = chunk_overhead - expected > 0 ? 1 : 0;
      int const e_chunk_sz = chunk_tmp + e_inc;
      int const e_begin = e_inc ? expected * e_chunk_sz
                                : expected * e_chunk_sz + chunk_overhead;
      MPI_Recv(a.data + e_begin * a.width, e_chunk_sz * JSIZE, MPI_DOUBLE,
               expected, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    }
    FILE *ff = fopen("result_parallel_common.dat", "w");
    for (int i = 0; i != a.height; ++i) {
      for (int j = 0; j != a.width; ++j)
        fprintf(ff, "%f ", getVal(a, i, j));
      fprintf(ff, "\n");
    }
    fclose(ff);
  } else
    MPI_Send(a.data, a.height * a.width, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD);

  releaseMatrix(a);
  MPI_Finalize();

  return 0;
}