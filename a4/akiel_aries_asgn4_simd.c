#include <immintrin.h>
#include <math.h>
#include <stdio.h>
#include <time.h>

double function_to_parallelize(int N, int M) {
    long i, j;
    long A = 0;
    double B = 0, C = 0, D = 0;

    // Calculate A
    for (i = 0; i < N; i++) {
        for (j = 0; j < M; j++) {
            A += i * j;
        }
    }

    // Calculate B
    __m256d sumB = _mm256_set1_pd(0.0);
    for (i = 1; i < (long)sqrt(A); i += 4) {
        __m256d vecI = _mm256_setr_pd(i, i + 1, i + 2, i + 3);
        sumB = _mm256_add_pd(sumB, _mm256_div_pd(_mm256_set1_pd(1.0), vecI));
    }
    double B_array[4];
    _mm256_storeu_pd(B_array, sumB);
    for (i = 0; i < 4; i++) {
        B += B_array[i];
    }

    // Calculate D
    double *D_values = (double *)malloc(M * N * sizeof(double));
    for (i = 0; i < M * N; i++) {
        for (j = 0; j < M; j++) {
            D_values[i] = pow(0.1, i * j);
        }
    }

    // Use Intel Intrinsics for parallel sum
    __m256d sumD = _mm256_set1_pd(0.0);
    for (i = 0; i < M * N; i += 4) {
        __m256d vecD = _mm256_loadu_pd(&D_values[i]);
        sumD = _mm256_add_pd(sumD, vecD);
    }
    double D_array[4];
    _mm256_storeu_pd(D_array, sumD);
    for (i = 0; i < 4; i++) {
        D += D_array[i];
    }

    free(D_values);

    // Calculate C
    for (i = 0; i < (long)B * (N + 1); i++) {
        for (j = 1; j < (long)sqrt(D); j++) {
            C += i / (double)j;
        }
    }

    double result = A + B - C / D;
    printf("Result: %ld + %f - %f / %f = %lf\n", A, B, C, D, result);
    return result;
}

int main() {
    clock_t start_time, end_time;

    start_time = clock();

    double result = function_to_parallelize(200, 200);

    end_time = clock();

    double cpu_time_used = ((double)(end_time - start_time)) / CLOCKS_PER_SEC;

    printf("result=%f\n", result);
    printf("Execution Time: %f seconds\n", cpu_time_used);

    return 0;
}
