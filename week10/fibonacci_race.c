#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <omp.h>
#include <sys/time.h>

long long* fibonacci(int K)
{
    long long* fib = malloc(sizeof(long long) * (K + 1));

    fib[0] = 0;
    fib[1] = 1;

    // #pragma omp paralell for num_threads(2)
    for (int i = 2; i < K + 1; i++)
    // {   
    //     #pragma omp critical
        // {
            fib[i] = fib[i - 2] + fib[i - 1];
    //     }
    // }

    return fib;
}

int main()
{
    int K = 1000;
    long long* fib;
    int sample = 1000000;

    double t, dt = 0;

    for (int i = 0; i < sample; i++)
    {
        t = omp_get_wtime();
        fib = fibonacci(K);            
        dt += (omp_get_wtime() - t) * 1e6;
    }
        
    printf("fibonacci(%d)=%lld, time=%0.6f\n", K, *(fib + K), dt / sample);

    return 0;
}
