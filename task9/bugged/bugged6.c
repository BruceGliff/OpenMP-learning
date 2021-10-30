/******************************************************************************
* ЗАДАНИЕ: bugged6.c
* ОПИСАНИЕ:
*   Множественные ошибки компиляции
******************************************************************************/

#include <omp.h>
#include <stdio.h>
#include <stdlib.h>

#define VECLEN 100

float a[VECLEN], b[VECLEN];

int main (int argc, char *argv[])
{
    int i;
    float sum;

    for (i = 0; i < VECLEN; i++)
        a[i] = b[i] = 1.0 * i;
    sum = 0.0;

    #pragma omp parallel shared(sum)
    {

        int tid = omp_get_thread_num();
        #pragma omp for reduction(+:sum) // sum has been private as it has the automatic storage duration, therefore - declared as private
        for (i = 0; i < VECLEN; i++)
        {
            sum = sum + (a[i] * b[i]);
            printf("  tid= %d i=%d\n", tid, i);
        }
    }
    printf("Sum = %f\n",sum); // by def sum will be 0.0 as function not able to see/change this var
}


