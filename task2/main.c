#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include <assert.h>

unsigned getCPUVal(int N) {
  unsigned sum = 0;
  for (int i = 1; i <= N; ++i)
    sum += (unsigned)i;
  return sum;
}

typedef struct Parser {
  int N;
  int fCheck;
} Parser;

void parseInput(int argc, char *argv[], Parser * Out) {
  assert(Out);
  if (argc < 2) {
    printf("N did not stated!\n");
    exit(-1);
  }
  int N = atoi(argv[1]);
  Out->N = N < 0 ? -N : N; 
  Out->fCheck = 0;
  if (argc > 2) {
    int fCheck = atoi(argv[2]);
    Out->fCheck = fCheck; // if argv[2] stated except 0 -> calculate check value
  }
}

int main(int argc, char *argv[]) {
  
  // Just parsing input.
  // The first param is N.
  // The second is linear CPU check
  Parser p;
  parseInput(argc, argv, &p);

  // Summing..
  unsigned sum = 0;
  int i = 0;
  #pragma omp parallel for reduction(+:sum) schedule(dynamic)
  for(i = 1; i <= p.N; ++i) {
    sum += (unsigned)i;
  }
  
  // Checking value
  unsigned CheckVal = 0;
  if (p.fCheck) {
    CheckVal = getCPUVal(p.N);
    printf("Check val: %u ", CheckVal);
  }
  printf("Sum: %u\n", sum);
  return  p.fCheck ? CheckVal - sum : 0;
}
