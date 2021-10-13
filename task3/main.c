#include <omp.h>
#include <stdio.h>
#include <string.h>

#define SIZE 100000

void FillArray(float* a);
void LinearCalc(float *a);
void Out(float *a);

int main() {
  float *a = (float*) malloc (sizeof(float) * SIZE);
  FillArray(a);
  // Calc an array to compare
  float *linear_a = (float*) malloc (sizeof(float) * SIZE);
  memcpy(linear_a, a, sizeof(float) * SIZE);
  LinearCalc(linear_a);
  
  // Preparation to chunk calculation
  int const max = omp_get_max_threads();
  int const chunk_tmp = SIZE / max;
  int const chunk_overhead = SIZE % max;

  omp_lock_t lock;
  omp_init_lock(&lock);

  // When parallal calculation is useless
  if (SIZE <= max * 3) {
    LinearCalc(a);
    Out(a);
    return 0;
  }


  int available_tid = 0;
  #pragma omp parallel for schedule(static)
  for (int i = 0; i < SIZE; ++i) {
    int const tid = omp_get_thread_num();

    int const inc = chunk_overhead - tid > 0 ? 1 : 0;
    int const chunk_sz = chunk_tmp + inc;
    int const begin = inc ? tid * chunk_sz : tid * chunk_sz + chunk_overhead;
    int const end = begin + chunk_sz - 1;

    //printf("TID: %d, MAX: %d, i: %d\n  begin: %d, chunk: %d, inc: %d\n", tid, max, i, begin, chunk_sz, inc);

    if (i == begin)
      if (tid == 0)
        a[i] *= a[i+1] / 3;
      else {
        // wait till a[i-1] calculated
        char exit = 0;
        do {
          // Wait till a[i-1] is calculated
          omp_set_lock(&lock);
          if (available_tid == tid) {
            float prev = a[i-1];
            a[i] *= a[i+1] * prev / 3;
            exit = 1;
          }
          omp_unset_lock(&lock);
        } while (!exit);
      }
    else if (i == end)
      if (tid == max - 1)
        a[i] *= a[i-1] / 3;
      else {
          float next = a[i+1];
          a[i] *= a[i-1] * next / 3;
          available_tid = tid + 1;
      }
    else {
      a[i] *= a[i+1] * a[i-1] / 3;
    }
  }

  // Comparation
  for (int i = 0; i != SIZE; ++i)
    if (a[i] != linear_a[i]) {
      printf("Missmatching on pos: %d. expected [%f], gotted [%f]\n", i, linear_a[i], a[i]);
      return -1;
    }

  Out(a);
  omp_destroy_lock(&lock);
  free(a);
  free(linear_a);
  return 0;
}

void FillArray(float* a) {
  for (int i = 0; i != SIZE; ++i)
    a[i] = (float)i+1;
}

void LinearCalc(float * a) {
  for (int i = 0; i != SIZE; ++i) {
    if (i == 0)
      a[i] *= a[i+1] / 3;
    else if (i == SIZE-1)
      a[i] *= a[i-1] / 3;
    else
      a[i] *= a[i+1] * a[i-1] / 3;
  }
}

void Out(float * a) {
  for (int i = 0; i != SIZE; ++i)
    printf("%f ", a[i]);
  printf("\n");
}
