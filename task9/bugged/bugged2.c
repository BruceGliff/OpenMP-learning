/******************************************************************************
* ЗАДАНИЕ: bugged2.c
* ОПИСАНИЕ:
*   Еще одна программа на OpenMP с багом. 
******************************************************************************/

#include <omp.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char **argv)
{
    int nthreads, i, tid;
    float total = 0.0;

    #pragma omp parallel shared(total) private(tid) // do private tid
    {
        tid = omp_get_thread_num();
        if (tid == 0)
        {
            nthreads = omp_get_num_threads();
            printf("Number of threads = %d\n", nthreads);
        }
        printf("Thread %d is starting...\n", tid);
        
        #pragma omp for schedule(dynamic, 10) reduction(+: total) // reduction for total
        for (i = 0; i < 100000; i++) // believe in huge i float misscalculation occurs.
            total = total + i*1.0;

        printf ("Thread %d is done! Total= %e\n", tid, total);
    }
}
