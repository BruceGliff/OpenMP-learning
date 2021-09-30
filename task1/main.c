#include "stdio.h" 
#include <omp.h>


int main(int argc, char *argv[]) {
  int next_to_print = 0;
  omp_lock_t lock;
  omp_init_lock(&lock);

  unsigned max = omp_get_max_threads();
  printf("Max threads: %u\n", max); 

  next_to_print = max - 1;
  
  #pragma omp parallel shared(next_to_print)
  {
    unsigned tid = omp_get_thread_num();

    char exit = 0;
    do {
      omp_set_lock(&lock);
      if (next_to_print == tid) {
        printf("%d\n", tid);
        fflush(NULL);
        --next_to_print;
        exit = 1;
      }
      omp_unset_lock(&lock);
    } while (!exit);
  }

  omp_destroy_lock(&lock);

  return(0);
}
