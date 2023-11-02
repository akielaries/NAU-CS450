#include <math.h>
#include <stdio.h>
#include <sys/time.h>
#include <time.h>
#include <unistd.h>

double function_to_parallelize(int N, int M) {
    long i, j;
    long A = 0;
    double B = 0, C = 0, D = 0;

    for (i = 0; i < N; i++) {
        for (j = 0; j < M; j++) {
            A += i * j;
            //printf("%ld * %ld A += %ld\n", i, j, A);
        }
    }

    for (i = 1; i < (long)sqrt(A); i++) {
        B += 1 / i;
        //printf("1 / %ld B += %f\n", i, B);
    }

    for (i = 0; i < M * N; i++)
        for (j = 0; j < M; j++) {
            D += pow(0.1, i * j);
            //printf("D += %lf\n", D);
        }

    for (i = 0; i < (long)B * (N + 1); i++)
        for (j = 1; j < (long)sqrt(D); j++) {
            C += i / j;
            //printf("C += %lf\n", C);
        }

    double result = A + B - C / D;
    printf("Result: %ld + %f - %f / %f = %lf\n", A, B, C, D, result);
    return result;

    // return A + B - C / D;
}

int main() {
    // you may add some code here to measure the execution time
    clock_t start_time, end_time;

    start_time = clock();

    double result = function_to_parallelize(200,200);

    end_time = clock();

    double cpu_time_used = ((double)(end_time - start_time)) / CLOCKS_PER_SEC;

    printf("result=%f\n", result);
    printf("Execution Time: %f seconds\n", cpu_time_used);

    return 0;
}
