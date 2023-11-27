// Akiel Aries aba275
//
// Not sure how this is supposed to be done with what we've learned
// I figured each looped just needed to be parallelized but i only
// added #pragmas to a few of the loops instead from trial and error
// on which is faster and without having to test each single combination
// of numbers for N and M and creating some additional overhead with
// conditionals...
//
// Increasing the number of threads on openMP just created more overhead
// meaning M & N must be greater than whatever overhead we are facing
// and M & N being from 50 - 300, this doesn't outweigh the time taken
// to spawn threads so I am unsure how we are supposed to do this...

#include <math.h>
#include <omp.h>
#include <stdio.h>
//#include <sys/time.h>
#include <time.h>

double function_to_parallelize(int N, int M) {
    long i, j;
    long A = 0;
    double B = 0, C = 0, D = 0;

    for (i = 0; i < N; i++) {
        for (j = 0; j < M; j++) {
            A += i * j;
        }
    }

    for (i = 1; i < (long)sqrt(A); i++) {
        B += 1 / i;
    }

    for (i = 0; i < M * N; i++) {
        for (j = 0; j < M; j++) {
            D += pow(0.1, i * j);
        }
    }

#pragma omp parallel for private(i, j) reduction(+ : C) schedule(static)
    for (i = 0; i < (long)B * (N + 1); i++) {
        for (j = 1; j < (long)sqrt(D); j++) {
            C += i / j;
        }
    }

    double result = A + B - C / D;
    printf("Result: %ld + %f - %f / %f = %lf\n", A, B, C, D, result);
    return result;
}

int main() {
    // the number of threads may stay deterministic
    omp_set_num_threads(2);

    // you may add some code here to measure the execution time
    clock_t start_time, end_time;

    start_time = clock();

    double result = function_to_parallelize(200, 300);

    end_time = clock();

    double cpu_time_used = ((double)(end_time - start_time)) / CLOCKS_PER_SEC;

    printf("result=%f\n", result);
    printf("Execution Time: %f seconds\n", cpu_time_used);

    return 0;
}
