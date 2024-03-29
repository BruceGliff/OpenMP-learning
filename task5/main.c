# include <math.h>
# include <omp.h>
# include <stdio.h>
# include <stdlib.h>
# include <string.h>

#define PI 3.14159265358979323846264338327950288419716939

double run(unsigned long long n);
double run_e(unsigned long long n, double x);

double factorial(int a) {
    double res = 1.0;
    for (int i = 2; i <= a; ++i)
        res *= i;
    return res;
}

int main (int argc, char *argv[]) {
  if (argc < 2) {
    printf("N does not stated!\n");
    return -1;
  }
  double x = 2.5;
  unsigned long long const N = atoll(argv[1]);
  double const e = run_e(N, x);
  printf("Calculated: e^%g = %g\n", x, e);
  double const pi = run(N);
  printf("Calculated: %g\nError: %g\n", pi, fabs(PI - pi));

  return 0;
}


double run_e(unsigned long long n, double x) {
  double res = 0;
  // calculationg of e in power if x.
#pragma omp parallel
#pragma omp for reduction (+: res)
  for (int i = 0; i <= n; ++i) {
    double const fac = factorial(i);
    // overcalculating of double.
    double const add = i > 774 ? 0.0 : pow(x, i) / fac;
    res += add;
  }
  return res;
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
