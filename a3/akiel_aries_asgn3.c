/**
 * Akiel Aries aba275
 * I've had to do a few monte carlo related simulations for benchmarking
 * purposes so I used similar logic/idea from that
 */
#include <math.h>
#include <omp.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define PI (3.141592653589793)

double f(double x) {
    return 1 / (sqrt(2 * PI)) * exp(-0.5 * x * x);
}

double calc_area(double x0, double x1, long points) {
    // points that fall within our x0 and x1
    int successes = 0;

// paralell region to caclulate number of successes (points within the range)
#pragma omp parallel for reduction(+ : successes)
    for (long i = 0; i < points; i++) {
        // the range of x0 -> x1 default should be 1
        double range = x1 - x0;
        // random x value, taken from ex_pi_monte_carlo_0.c and my own monte
        // carlo efforts
        double rand_x = ((double)rand() / RAND_MAX);
        // random y value
        double rand_y = ((double)rand() / RAND_MAX);

        // random x value based on the ranges provided
        double x = x0 + rand_x * range;
        // random y value using the distribution function above
        double y = rand_y * f(0);

        // check whether the generated points lie within the density function's
        // curve
        if (y <= f(x)) {
            // this could probably be atomic but the result is the same
            // regardless
            successes++;
        }
    }
    // calculate area simlar to how assignment one was operating
    double area = (x1 - x0) * f(0);
    // calculate CDF based on number of points within the curve/total points
    double cdf = (double)successes / points;

    // esitmated CDF * the calculated area under the curve
    return cdf * area;
}

int main(int argc, char *argv[]) {
    // default values of arguments:
    // 1-2) range (lower and upper bounds)
    double x0 = 0, x1 = 1;
    // 3) the number of points
    long points = 1e8;
    // 4) the number of threads
    int threads = 1;

    // read arguments from console
    // assume that correct values and correct number of arguments were provided
    if (argc > 1) {
        x0 = atof(argv[1]);
        x1 = atof(argv[2]);
        points = atof(argv[3]);
        threads = atoi(argv[4]);
    }

    printf("lower bound = %f\n", x0);
    printf("upper bound = %f\n", x1);
    printf("points total = %d\n", points);
    printf("threads = %d\n", threads);

    omp_set_num_threads(threads);
    double area = calc_area(x0, x1, points);
    printf("max pdf value = %f\n", f(0));
    printf("estimated cdf = %f\n", area);

    return 0;
}
