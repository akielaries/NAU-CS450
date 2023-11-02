#include <omp.h>
#include <stdio.h>

#define SIZE 1000
#define THREADS 3

int main()
{
    long sum = 0;
    int arr[SIZE];
    int i;
    
    for (i = 0; i < SIZE; i++) arr[i] = i + 1;
    
    // int th;
    // int chunk = SIZE / THREADS;

    // int th_sum[THREADS];
    // for (i = 0; i < THREADS; i++) th_sum[i] = 0;

    // calculate sum
    #pragma omp parallel for num_threads(THREADS) reduction(+: sum)
    // {
        // th = omp_get_thread_num();
        // int th_sum = 0;

        // #pragma omp for
        for (i = 0; i < SIZE; i++)
        {
            sum += arr[i];
        }
        // threat of race condition
        // #pragma omp atomic
        //     sum += th_sum;
    // }

    // for (i = 0; i < THREADS; i++) sum += th_sum[i];

    printf("sum=%d, check=%d\n", sum, (SIZE + 1) * SIZE / 2);
}