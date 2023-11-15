#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <omp.h>

#define SIZE 1000

int main()
{
    long i, j;
    int* a = malloc(sizeof(int) * SIZE);
    int* b = malloc(sizeof(int) * SIZE);
    int* c = malloc(sizeof(int) * SIZE);
    int low, high;

    // #pragma omp parallel for
    for (i = 0; i < SIZE; i++)
    {
        // usleep(1);
        a[i] = i;
        b[i] = SIZE - i;
    }

    double t = 0;
    for (int k = 0; k < 50; k++)
    {
        double t0 = omp_get_wtime();

        #pragma omp parallel private(i, j)
        for (i = 0; i < SIZE; i++)
        {
            usleep(1);
            low = a[i];
            high = b[i];
            if (low > high)
            {
                #pragma omp single
                printf("Exit iteration %d %ld\n", k, i);
                break;
            }
            #pragma omp barrier

            #pragma omp for nowait
            for (j = low; j < high; j++)
            {
                usleep(1);
                c[j] = (c[j] - a[i]) / b[i];
            }
        }

        t += omp_get_wtime() - t0;
    }

    printf("Execution mean time: %.3f\n", t / 50);

    return 0;
}