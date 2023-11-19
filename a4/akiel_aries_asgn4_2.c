#include <math.h>
#include <omp.h>
#include <stdio.h>
#include <sys/time.h>
#include <time.h>
#include <unistd.h>

double function_to_parallelize(int N, int M) {
    long i, j;
    long A = 0;
    double B = 0, C = 0, D = 0;

#pragma omp parallel for reduction(+ : A) private(j) schedule(static)
    for (i = 0; i < N; i++) {
        for (j = 0; j < M; j++) {
            A += i * j;
        }
    }

#pragma omp parallel for reduction(+ : B) schedule(static)
    for (i = 1; i < (long)sqrt(A); i++) {
        B += 1.0 / i; // Corrected to use 1.0 instead of 1 to force
                      // floating-point division
    }

#pragma omp parallel for reduction(+ : D) schedule(static)
    for (i = 0; i < M * N; i++) {
        for (j = 0; j < M; j++) {
            D += pow(0.1, i * j);
        }
    }

#pragma omp parallel for reduction(+ : C) schedule(static)
    for (i = 0; i < (long)B * (N + 1); i++) {
        for (j = 1; j < (long)sqrt(D); j++) {
            C += i / (double)j; // Corrected to use double for j
        }
    }

    double result = A + B - C / D;
    printf("Result: %ld + %f - %f / %f = %lf\n", A, B, C, D, result);
    return result;
}

int main() {
    // you may add some code here to measure the execution time
    clock_t start_time, end_time;

    start_time = clock();

    // Set the number of threads to 2
    omp_set_num_threads(2);

    double result = function_to_parallelize(200, 300);

    end_time = clock();

    double cpu_time_used = ((double)(end_time - start_time)) / CLOCKS_PER_SEC;
    printf("result=%f\n", result);
    printf("Execution Time: %f seconds\n", cpu_time_used);

    return 0;
}
