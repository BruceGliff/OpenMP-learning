#include <omp.h>
#include <stdio.h>
#include <string.h>

#define SIZE 12

void FillArray(float* a);
void LinearCalc(float *a);
void Out(float *a);

int main() {

  float *a = (float*) malloc (sizeof(float) * SIZE);
  FillArray(a);
  float *linear_a = (float*) malloc (sizeof(float) * SIZE);
  memcpy(linear_a, a, sizeof(float) * SIZE);
  Out(linear_a);
  LinearCalc(linear_a);
  
  int tid_to_process = 0;
  int const max = omp_get_max_threads();
  int const chunk_tmp = SIZE / max;
  int const chunk_overhead = SIZE % max;

  if (SIZE <= max * 3 && 0) {
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
        while(available_tid != tid);
        #pragma omp critical (GetPrev)
        {
          float prev = a[i-1];
          a[i] *= a[i+1] * prev / 3;
        }
      }
    else if (i == end)
      if (tid == max - 1)
        a[i] *= a[i-1] / 3;
      else {
        #pragma omp critical (GetNext)
        {
          float next = a[i+1];
          a[i] *= a[i-1] * next / 3;
          available_tid = tid + 1;
        }
      }
    else {
      a[i] *= a[i+1] * a[i-1] / 3;
    }
  }

  Out(a);
  Out(linear_a);

  // float elm = 1.f;
  // int idx_to_inc = chunk_overhead;
  // for (int i = 0; i != SIZE; ++i) {
  //   int const inc = idx_to_inc ? 1 : 0;
  //   int const chunk_sz = chunk_tmp + inc;
  //   int const tid = inc ? i / chunk_sz  : (i - chunk_overhead) / chunk_sz;
  //   int const begin = inc ? tid * chunk_sz : tid * chunk_sz + chunk_overhead;
  //   int const end = begin + chunk_sz - 1;

  //   if (i == begin)
  //     elm = i == 0 ? 1.f : a[i-1];
  //   if (i == end)
  //     a[i] *= a[i+1];
  //   a[i] *= elm;

    
  //   if (i == end)
  //     idx_to_inc = idx_to_inc ? --idx_to_inc : 0;
  // }

  for (int i = 0; i != SIZE; ++i)
    if (a[i] != linear_a[i]) {
      printf("Missmatching on pos: %d. expected [%f], gotted [%f]\n", i, linear_a[i], a[i]);
      return -1;
    }

  Out(a);

}


void FillArray(float* a) {
  for (int i = 0; i != SIZE; ++i)
    a[i] = (float) i+1;
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
