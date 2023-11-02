#include <stdio.h>
#include <unistd.h>
#include <time.h>
#include <math.h>
#include <omp.h>
#include <sys/time.h>

double function_to_parallelize(int N, int M)
{
    long i, j;
    long A = 0;
    double B = 0, C = 0, D = 0;
    
    for (i = 0; i < N; i++)
    {
        for (j = 0; j < M; j++)
        {
            A += i * j;
        }
    }
 
    for (i = 1; i < (long)sqrt(A); i++)
        B += 1 / i;

    for (i = 0; i < M * N; i++)
        for (j = 0; j < M; j++)
        {
            D += pow(0.1, i * j);
        }

    for (i = 0; i < (long)B * (N + 1); i++)
        for (j = 1; j < (long)sqrt(D); j++)
        {
            C += i / j;
        }

    return A + B - C / D;
}

int main()
{
    // the number of threads may stay deterministic
    omp_set_num_threads(2);

    // you may add some code here to measure the execution time
    double start_time, end_time;
    
    start_time = omp_get_wtime();

    double result = function_to_parallelize(200, 200);
    
    end_time = omp_get_wtime();

    printf("result=%f\n", result);
    printf("Execution Time: %f seconds\n", end_time - start_time);

    return 0;
}
