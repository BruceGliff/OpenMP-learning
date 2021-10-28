#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <omp.h>

int main (int argc, char *argv[]) {
  if (argc < 2) {
    printf("N does not stated!\n");
    return -1;
  }
  int const N = atoll(argv[1]);
  switch (N)
  {
  case 0:
  case 1:
    printf("0\n");
    return 0;
  case 2:
    printf("1\n");
    return 0;  
  }
  int banned = 2;
  char * IsPrime = (char*) malloc(N);
  if (!IsPrime) {
    perror("malloc fails");
    return -1;
  }
  memset(IsPrime, 1, N);
  IsPrime[1]=0;
  for(int i = 2; i * i <= N; ++i)
    // for i = 2: 
    // i * 2, i * 3, i * 4 ... are defenetly not prime so we ban it
    // we counting not prime numbers, but non-prime
    #pragma omp parallel for
    for(int j = i * i; j <= N; j = j + i)
      if(IsPrime[j]) {
        #pragma omp atomic
        ++banned;
        IsPrime[j] = 0;
      }
  printf("%llu\n", N - banned);
  free(IsPrime);
  return 0;
}