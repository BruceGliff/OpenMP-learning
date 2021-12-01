#include <math.h>
#include <omp.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void run_through(double *res, int size, double **A, double *f, double *y_pr,
                 double *alpha_pr, double *betta_pr);
double calculate_b(int argc, int it, int iterations);

// Launching with any params gives results around b=1.5.
int main(int argc, char *argv[]) {
  int const iterations = 10;
  int const Points = 2000;
  double const Step = 1.0 / Points;
  unsigned max_th = omp_get_max_threads();

  //--------------------------------------------------------------------
  // BLOCK of allocs for each thread independent memory association.
  double **solution = (double **)malloc(max_th * sizeof(double *));
  for (int i = 0; i != max_th; ++i)
    solution[i] = (double *)malloc(Points * sizeof(double *));

  int const size = Points - 2;

  double **y = (double **)malloc(max_th * sizeof(double *));
  for (int i = 0; i != max_th; ++i)
    y[i] = (double *)malloc(size * sizeof(double *));

  double **f = (double **)malloc(max_th * sizeof(double *));
  for (int i = 0; i != max_th; ++i)
    f[i] = (double *)malloc(size * sizeof(double *));

  // Preparation of Jacobi's matrix.
  double ***A = (double ***)malloc(max_th * sizeof(double *));
  for (int i = 0; i != max_th; ++i) {
    A[i] = (double **)malloc(size * sizeof(double *));
    for (int j = 0; j != size; ++j)
      A[i][j] = (double *)malloc(size * sizeof(double));
  }

  // Allocs for run-through algorithm
  double **res_pr = (double **)malloc(max_th * sizeof(double *));
  for (int i = 0; i != max_th; ++i)
    res_pr[i] = (double *)malloc(size * sizeof(double *));

  double **y_pr = (double **)malloc(max_th * sizeof(double *));
  for (int i = 0; i != max_th; ++i)
    y_pr[i] = (double *)malloc(size * sizeof(double *));

  double **alpha_pr = (double **)malloc(max_th * sizeof(double *));
  for (int i = 0; i != max_th; ++i)
    alpha_pr[i] = (double *)malloc(size * sizeof(double *));

  double **betta_pr = (double **)malloc(max_th * sizeof(double *));
  for (int i = 0; i != max_th; ++i)
    betta_pr[i] = (double *)malloc(size * sizeof(double *));
    //--------------------------------------------------------------------

#pragma omp parallel for
  for (int it = 0; it <= iterations; ++it) {
    int const tid = omp_get_thread_num();
    // Calculate b in range 0.1 1.0 or
    // nearby 1.5 depends on argc.
    double const b = calculate_b(argc, it, iterations);
    // Gets thread-local memory.
    double *loc_solution = solution[tid];
    double *loc_y = y[tid];
    double *loc_f = f[tid];
    double **loc_A = A[tid];

    double *loc_res_pr = res_pr[tid];
    double *loc_y_pr = y_pr[tid];
    double *loc_alpha_pr = alpha_pr[tid];
    double *loc_betta_pr = betta_pr[tid];

    memset(loc_y, 0, size * sizeof(double));

    // Newton's method of Numer's approximation.
    for (int j = 0; j != 1000; ++j) {
      // usage of tridiagonal Jacobi matrix. Tread-local.
      for (int i = 0; i != size - 1; ++i) {
        loc_A[i][i] = -2 + 5.0 * Step * Step / 6.0 * exp(-loc_y[i]);
        loc_A[i + 1][i] = 1 + Step * Step / 12.0 * exp(-loc_y[i + 1]);
        loc_A[i][i + 1] = 1 + Step * Step / 12.0 * exp(-loc_y[i]);
      }
      loc_A[size - 1][size - 1] =
          -2 + 5.0 * Step * Step / 6.0 * exp(-loc_y[size - 1]);

      // Right vector of weights.
      loc_f[0] = loc_y[1] - 2 * loc_y[0] + 1 -
                 Step * Step *
                     (5.0 / 6.0 * exp(-loc_y[0]) + 1.0 / 12.0 * exp(-loc_y[1]) +
                      1.0 / 12.0 * exp(-1.0));
      loc_f[size - 1] =
          b - 2 * loc_y[size - 1] + loc_y[size - 2] -
          Step * Step *
              (5.0 / 6.0 * exp(-loc_y[size - 1]) + 1.0 / 12.0 * exp(-b) +
               1.0 / 12.0 * exp(-loc_y[size - 2]));

      for (int i = 1; i != size - 1; ++i)
        loc_f[i] =
            loc_y[i + 1] - 2 * loc_y[i] + loc_y[i - 1] -
            Step * Step *
                (5.0 / 6.0 * exp(-loc_y[i]) + 1.0 / 12.0 * exp(-loc_y[i + 1]) +
                 1.0 / 12.0 * exp(-loc_y[i - 1]));

      run_through(loc_res_pr, size, loc_A, loc_f, loc_y_pr, loc_alpha_pr,
                  loc_betta_pr);
      for (int i = 0; i != size; ++i)
        loc_y[i] -= loc_res_pr[i];
    }

    loc_solution[0] = 1.0;
    loc_solution[Points - 1] = b;
    for (int i = 0; i != size; ++i)
      loc_solution[i + 1] = loc_y[i];

    char file_name[256];
    int ch_size = sprintf(file_name, "res%d.dat", it);
    file_name[ch_size] = '\0';
    FILE *out = fopen(file_name, "w");
    for (int i = 0; i != Points; ++i)
      fprintf(out, "%lf\t%lf\n", Step * i, loc_solution[i]);
    fclose(out);
  }

  for (int i = 0; i != max_th; ++i)
    free(solution[i]);
  free(solution);

  for (int i = 0; i != max_th; ++i)
    free(y[i]);
  free(y);

  for (int i = 0; i != max_th; ++i)
    free(f[i]);
  free(f);

  for (int i = 0; i != max_th; ++i) {
    for (int j = 0; j != size; ++j)
      free(A[i][j]);
    free(A[i]);
  }
  free(A);

  for (int i = 0; i != max_th; ++i)
    free(res_pr[i]);
  free(res_pr);

  for (int i = 0; i != max_th; ++i)
    free(y_pr[i]);
  free(y_pr);

  for (int i = 0; i != max_th; ++i)
    free(alpha_pr[i]);
  free(alpha_pr);

  for (int i = 0; i != max_th; ++i)
    free(betta_pr[i]);
  free(betta_pr);
}

// Tridiagonal run-through.
void run_through(double *res, int size, double **A, double *f, double *y_pr,
                 double *alpha_pr, double *betta_pr) {
  y_pr[0] = A[0][0];
  alpha_pr[0] = -A[1][0] / y_pr[0];
  betta_pr[0] = f[0] / y_pr[0];

  // Forward run-through
  for (int i = 1; i < size - 1; ++i) {
    y_pr[i] = A[i][i] + A[i - 1][i] * alpha_pr[i - 1];
    alpha_pr[i] = -A[i + 1][i] / y_pr[i];
    betta_pr[i] = (f[i] - A[i - 1][i] * betta_pr[i - 1]) / y_pr[i];
  }

  y_pr[size - 1] =
      A[size - 1][size - 1] + A[size - 2][size - 1] * alpha_pr[size - 2];
  betta_pr[size - 1] =
      (f[size - 1] - A[size - 2][size - 1] * betta_pr[size - 2]) /
      y_pr[size - 1];

  // Backward run-through
  res[size - 1] = betta_pr[size - 1];

  for (int i = size - 2; i >= 0; --i)
    res[i] = alpha_pr[i] * res[i + 1] + betta_pr[i];
}

double calculate_b(int argc, int it, int iterations) {
  double const middle = 1.5;
  double const disp = 0.1;
  double const step = disp / iterations;

  return argc > 1 ? (middle - disp / 2) + step * it : (double)it / iterations;
}