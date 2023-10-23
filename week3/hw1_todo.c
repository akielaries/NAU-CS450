#include <math.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

#define PI (3.141592653589793)

struct arguments {
    // do not change the structure!
    double start;
    double finish;
    double precision;
    double *area;
};

double f(double x) {
    // returns density function for standard normal distribution
    return 1 / (sqrt(2 * PI)) * exp(-0.5 * x * x);
}

double trapeze_area(double a, double b, double h) {
    // return the area of a trapeze with bases a and b and height h
    return 0.5 * (a + b) * h;
}

void *calc(void *arg) {
    // calculations to be performed in a thread

    // put your code here

    return NULL;
}

double calc_area(double x0, double x1, double dx, int nthreads) {
    // caclulates the area under the curve between x0 and x1
    // with the precision dx by using nthreads threads

    double area = 0;

    // put your code here

    return area;
}

int main(int argc, char *argv[]) {
    // default values of arguments:
    // 1-2) range (lower and upper bounds)
    double x0 = 0, x1 = 1;
    // 3) precision
    double prec = 1e-6;
    // 4) number of threads used to perform calculations
    int nthreads = 1;

    // read arguments from console
    // assume that correct values and correct number of arguments were provided
    if (argc > 1) {
        x0 = atof(argv[1]);
        x1 = atof(argv[2]);
        prec = atof(argv[3]);
        nthreads = atoi(argv[4]);
    }

    printf("lower bound = %f\n", x0);
    printf("upper bound = %f\n", x1);
    printf("precision = %f\n", prec);
    printf("threads = %d\n", nthreads);

    double area = calc_area(x0, x1, prec, nthreads);
    printf("area = %f\n", area);

    return 0;
}