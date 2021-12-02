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
  // if there are any args -> do printing.
  int do_print = argc > 1;

  MPI_Init(NULL, NULL);

  int proc_num = 1;
  int rank = 0;

  MPI_Comm_size(MPI_COMM_WORLD, &proc_num);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);

  // There is the separation on proc_num tasks.
  int const chunk_tmp = ISIZE / proc_num;
  int const chunk_overhead = ISIZE % proc_num;
  int const proc_count = ISIZE > proc_num ? proc_num : ISIZE;
  int const inc = chunk_overhead - rank > 0 ? 1 : 0;
  int const chunk_sz = chunk_tmp + inc;
  int const begin = inc ? rank * chunk_sz : rank * chunk_sz + chunk_overhead;
  int const end = begin + chunk_sz - 1;

  matrix a;
  if (!rank)
    // Main process recived all matrix as it the one who will print.
    a = createMatrix(ISIZE, JSIZE);
  else
    // Other processes recived small matricies but big enough to calculate its
    // part.
    a = createMatrix(chunk_sz, JSIZE);

  int i, j;
  for (i = 0; i != chunk_sz; ++i) // Not a.height!
    for (j = 0; j != a.width; ++j)
      setVal(a, i, j, 10 * (i + begin) + j);

  // Calculating independent data.
  for (i = 0; i != chunk_sz; ++i) // Not a.height!
    for (j = 0; j != a.width; ++j)
      setVal(a, i, j, sin(0.00001 * getVal(a, i, j)));

  // Main process do results collecting and printing.
  if (!rank) {
    int expected = 0;
    // Recieving all calculated data from other processes.
    while (++expected != proc_count) {
      // Recalculating chunks of revieved packets.
      int const chunk_overhead = ISIZE % proc_num;
      int const e_inc = chunk_overhead - expected > 0 ? 1 : 0;
      int const e_chunk_sz = chunk_tmp + e_inc;
      int const e_begin = e_inc ? expected * e_chunk_sz
                                : expected * e_chunk_sz + chunk_overhead;
      MPI_Recv(a.data + e_begin * a.width, e_chunk_sz * a.width, MPI_DOUBLE,
               expected, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    }
    if (do_print) {
      FILE *ff = fopen("result_parallel_common.dat", "w");
      for (i = 0; i != a.height; ++i) {
        for (j = 0; j != a.width; ++j)
          fprintf(ff, "%f ", getVal(a, i, j));
        fprintf(ff, "\n");
      }
      fclose(ff);
    }
  } else
    // Sending local-calculated results.
    MPI_Send(a.data, a.height * a.width, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD);

  releaseMatrix(a);
  MPI_Finalize();

  return 0;
}
