#include <math.h>
#include <omp.h>
#include <stdio.h>
#include <sys/time.h>
#include <unistd.h>

void work(int N, int threads) {
    int j;
#pragma omp parallel for num_threads(threads) private(j)
    for (int i = 0; i < N; i++) {
        // some calculations
        j = i * i;
    }
}

int main() {
    int T = 8, L = 10, S = 50;
    int step = 5000;

    float durs[T][L];

    for (int th = 0; th < T; th++)
        for (int n = 0; n < L; n++) {
            float sum_dt = 0;
            for (int i = 0; i < S; i++) {
                double t = omp_get_wtime();
                work((n + 1) * step, th + 1);
                float dt = omp_get_wtime() - t;
                sum_dt += dt;
            }
            durs[th][n] = sum_dt / S;
        }

    for (int n = 0; n < L; n++) {
        for (int th = 0; th < T; th++)
            printf("n=%d th=%d time=%0.3f\n",
                   (n + 1) * step,
                   th + 1,
                   1e6 * durs[th][n]);
        printf("\n");
    }

    return 0;
}