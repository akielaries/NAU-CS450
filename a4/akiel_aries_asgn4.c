#include <stdio.h>
#include <time.h>
#include <math.h>
#include <omp.h>
#include <sys/time.h>

double function_to_parallelize(int N, int M)
{
    long i, j;
    long A = 0;
    double B = 0, C = 0, D = 0;

    // Parallelize the outer loop
    #pragma omp parallel for reduction(+:A) private(j)
    for (i = 0; i < N; i++)
    {
        for (j = 0; j < M; j++)
        {
            A += i * j;
        }
    }

    #pragma omp parallel for reduction(+:B)
    for (i = 1; i < (long)sqrt(A); i++)
        B += 1 / i;

    #pragma omp parallel for reduction(+:D) private(j)
    for (i = 0; i < M * N; i++)
    {
        for (j = 0; j < M; j++)
        {
            D += pow(0.1, i * j);
        }
    }

    #pragma omp parallel for reduction(+:C)
    for (i = 0; i < (long)B * (N + 1); i++)
    {
        for (j = 1; j < (long)sqrt(D); j++)
        {
            C += i / j;
        }
    }

    return A + B - C / D;

}

int main()
{
    // the number of threads may stay deterministic
    omp_set_num_threads(2);

    // you may add some code here to measure the execution time
    clock_t start_time, end_time;

    start_time = clock();

    double result = function_to_parallelize(200, 200);

    end_time = clock();

    double cpu_time_used = ((double)(end_time - start_time)) / CLOCKS_PER_SEC;

    printf("result=%f\n", result);
    printf("Execution Time: %f seconds\n", cpu_time_used);

    return 0;
}

