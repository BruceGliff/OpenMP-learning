/* C implementation QuickSort */
#include <math.h>
#include <omp.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

void swap(int *x, int *y);
int partition(int *arr, int l, int r);
void quickSort(int *arr, int l, int r);
void print(int *arr, int size);
int checkForSorted(int *arr, int size);
int checkForReverseSorted(int *arr, int size);

int main(int argc, char *argv[]) {
  int const ARRAY_MAX_SIZE = 10000;
  int arr[ARRAY_MAX_SIZE];
  int Size = 0;

  if (argc == 1) {
    // input from console
    int var = 0;
    while (scanf("%d", arr + Size++) != EOF)
      ;
  } else {
    // input from file
    FILE *f_in = fopen(argv[1], "r");
    if (!f_in) {
      printf("%s\n", argv[1]);
      perror("No file");
      return -1;
    }
    int var = 0;
    while (fscanf(f_in, "%d", arr + Size++) != EOF)
      ;
    fclose(f_in);
  }
  Size--;

  // corner cases.
  if (Size == 1) {
    print(arr, Size);
    return -1;
  }
  if (checkForSorted(arr, Size))
    return -1;
  if (checkForReverseSorted(arr, Size))
    return -1;

// Launches parallel section.
#pragma omp parallel
// Single launch of the inital state.
#pragma omp single nowait
  quickSort(arr, 0, Size - 1);

  print(arr, Size);
  return 0;
}

void swap(int *x, int *y) {
  int tmp = *x;
  *x = *y;
  *y = tmp;
}
int division(int *arr, int l, int r) {
  int pivot = arr[r];
  int idx = (l - 1);
  for (int i = l; i != r; ++i)
    if (arr[i] <= pivot)
      swap(&arr[++idx], &arr[i]);
  swap(&arr[++idx], &arr[r]);
  return idx;
}

// Devisions array into subarrays and puts values less then pivot into one part
// and greater int another.
void quickSort(int *arr, int l, int r) {
  if (l < r) {
    int piv_idx = division(arr, l, r);
#pragma omp task
    quickSort(arr, l, piv_idx - 1);
#pragma omp task
    quickSort(arr, piv_idx + 1, r);
  }
}

void print(int *arr, int size) {
  for (int i = 0; i != size; ++i)
    printf("%d ", arr[i]);
  printf("\n");
}

int checkForSorted(int *arr, int size) {
  for (int i = 0; i != size - 1; ++i)
    if (arr[i] > arr[i + 1])
      return 0;

  print(arr, size);
  return 1;
}

int checkForReverseSorted(int *arr, int size) {
  for (int i = 0; i != size - 1; ++i)
    if (arr[i] < arr[i + 1])
      return 0;

  for (int i = size - 1; i >= 0; --i)
    printf("%d ", arr[i]);
  printf("\n");
  return 1;
}