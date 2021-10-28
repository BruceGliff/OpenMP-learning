# include <math.h>
# include <omp.h>
# include <stdio.h>
# include <stdlib.h>
# include <string.h>

#define PI 3.14159265358979323846264338327950288419716939

double run(unsigned long long n);

int main (int argc, char *argv[]) {
  if (argc < 2) {
    printf("N does not stated!\n");
    return -1;
  }
  unsigned long long const N = atoll(argv[1]);
  double const pi = run(N);
  printf("Calculated: %g\nError: %g\n", pi, fabs(PI - pi));

  return 0;
}

double run(unsigned long long n) {
  double res = 0.0;
  double loc_res = 0.0;
  double prev = 0.0;

// Calculation PI vai series.
#pragma omp parallel private (prev)
#pragma omp for reduction (+: loc_res)
  for (unsigned long long i = 1; i <= n; ++i) {
    prev = (double) (2 * i - 1) / (double) (2  * n);
    loc_res = loc_res + 1.0 / ( 1.0 + prev * prev );
  }
  res = 4.0 * loc_res / (double) n;

  return res;
}