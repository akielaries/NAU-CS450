// sleep to show load imbalance as "work"
#include <unistd.h>
#include <stdlib.h>
#include <omp.h>
#include <stdio.h>
#include <sys/time.h>

#define NTHREADS 4
#define N 20

int main()
{
    omp_set_num_threads(NTHREADS);		
    int i, tid;
    struct timeval start, end;
    double elapsed;

    gettimeofday(&start, NULL);

    // static schedule by default
    #pragma omp parallel for private(tid, i) schedule(runtime)
    for (i = 0; i < N; i++)
    {
        tid = omp_get_thread_num();
        usleep(1e5 * i); // sleep for i * 0.1 seconds
        printf("Thread %d is doing iteration %d\n", tid, i);
    }

    gettimeofday(&end, NULL);

    elapsed = ((end.tv_sec * 1e6 + end.tv_usec) -
              (start.tv_sec * 1e6 + start.tv_usec)) / 1e6;

    printf("Elapsed time: %.3f\n", elapsed);

    return 0;
}