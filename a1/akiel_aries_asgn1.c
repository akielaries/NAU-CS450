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
    // struct of arguments
    struct arguments *args = (struct arguments *)arg;
    // point to the args
    double start = args->start;
    double finish = args->finish;
    double precision = args->precision;

    // declare precision, start and local area
    double h = precision;
    double x = start;
    double local_area = 0.0;

    // traverse start to finish
    while (x < finish) {
        // calculate density at x
        double a = f(x);
        // calculate density at x + h
        double b = f(x + h);
        // append local_area with result of trapeze_area()
        local_area += trapeze_area(a, b, h);
        // move start variable to shift
        x += h;
    }
    double *result = (double *)malloc(sizeof(double));
    if (result == NULL) {
        perror("Memory allocation error");
        exit(EXIT_FAILURE);
    }
    
    *result = local_area;
    args->area = result;

    return result;
}

double calc_area(double x0, double x1, double dx, int nthreads) {
    // caclulates the area under the curve between x0 and x1
    // with the precision dx by using nthreads threads

    // total area
    double area = 0;
    // array of pthread_t
    pthread_t threads[nthreads];
    // array of arguments for threads
    struct arguments args[nthreads];

    // calculate the range for each thread
    double range = (x1 - x0) / nthreads;

    // traverse threads
    for (int i = 0; i < nthreads; i++) {
        // start/end point intervals for the threads
        args[i].start = x0 + i * range;
        args[i].finish = x0 + (i + 1) * range;
        // the intervals precision defined from args
        args[i].precision = dx;
        // create the new thread with assignment to calc() to execute
        pthread_create(&threads[i], NULL, calc, &args[i]);
    }

    // wait for all threads to finish
    for (int i = 0; i < nthreads; i++) {
        double *result;
        pthread_join(threads[i], (void **)&result);
        // accumulate the local areas to get the total area
        //area += *(args[i].area);
        area += *result;
        free(result);
    }

    // return total area
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
