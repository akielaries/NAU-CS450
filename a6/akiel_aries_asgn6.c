// Akiel Aries aba275

#include <math.h>
#include <omp.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#define SAMPLE 100
// methods to calculate the result: pthreads or openmp tasks
typedef enum { pthreads, tasks } method;
// problem parameters
struct parameters {
    double start;     // lower bound
    double finish;    // upper bound
    double precision; // precision
    int nthreads;     // number of threads
};
// arguments to be passed to the calculations routine used by pthreads
struct arguments {
    double start;
    double finish;
    double precision;
    double *area;
};
// density function of the standard normal distribution
double f(double x) {
    return 1 / (sqrt(2 * M_PI)) * exp(-0.5 * x * x);
}
double trapeze_area(double a, double b, double h) {
    return 0.5 * (a + b) * h;
}
void *calc(void *arg) {
    struct arguments *argument;
    double start, finish, precision;
    double *area;
    argument = (struct arguments *)arg;
    // unpack arguments
    start = argument->start;
    finish = argument->finish;
    precision = argument->precision;
    area = argument->area;
    *area = 0;
    for (double i = start; i < finish; i += precision)
        *area += trapeze_area(f(i), f(i + precision), precision);
    return NULL;
}
double calc_area_pthreads(struct parameters params) {
    int t;
    double area = 0, areas[params.nthreads];
    pthread_t thread[params.nthreads];
    double width = (params.finish - params.start) / params.nthreads;
    // set arguments for threads
    struct arguments *args[params.nthreads];
    for (t = 0; t < params.nthreads; t++) {
        args[t] = (struct arguments *)calloc(1, sizeof(struct arguments));
        args[t]->start = params.start + width * t;
        args[t]->finish = params.start + width * (t + 1);
        args[t]->precision = params.precision;
        args[t]->area = &(areas[t]);
    }
    // create threads
    for (t = 0; t < params.nthreads; t++)
        pthread_create(&thread[t], NULL, calc, (void *)args[t]);
    // join threads
    for (t = 0; t < params.nthreads; t++)
        pthread_join(thread[t], NULL);
    // calculate total area
    for (t = 0; t < params.nthreads; t++)
        area += *args[t]->area;
    return area;
}
double calc_area_tasks(struct parameters params) {
    double area = 0;
    double precision = params.precision;
    double start = params.start;
    double finish = params.finish;

    #pragma omp parallel for reduction(+:area)
    for (int i = 0; i < (int)((finish - start) / precision); i++) {
        double x = start + i * precision;
        area += trapeze_area(f(x), f(x + precision), precision);
    }

    return area;
}


// returns the average execution time based on the empirical data
// m - the method used to calculate the result
// size - sample size
// params - parameters to calculate the result
double experiment(method m, int size, struct parameters params) {
    double t, times = 0;
    for (int i = 0; i < size; i++) {
        t = omp_get_wtime();
        if (m == pthreads)
            calc_area_pthreads(params);
        else
            calc_area_tasks(params);
        times += omp_get_wtime() - t;
    }
    times /= size;
    return times;
}
// the simplified estimation of speedup
double speedup(double seq_time, double par_time) {
    return par_time != 0 ? seq_time / par_time : INFINITY;
}
int main(int argc, char *argv[]) {
    // default values of arguments:
    struct parameters params = {0, 1, 1e-6, 1};
    // read arguments from console
    // assume that correct values and correct number of arguments were provided
    if (argc > 1) {
        params.start = atof(argv[1]);
        params.finish = atof(argv[2]);
        params.precision = atof(argv[3]);
        params.nthreads = atoi(argv[4]);
    }
    printf("lower bound = %f\n", params.start);
    printf("upper bound = %f\n", params.finish);
    printf("precision = %f\n", params.precision);
    printf("threads = %d\n", params.nthreads);
    // calculate area under the curve
    double area_pthreads = calc_area_pthreads(params);
    double area_tasks = calc_area_tasks(params);
    printf("area = %f, %f\n", area_pthreads, area_tasks);
    // estimate execution time
    double time_pthreads = experiment(pthreads, SAMPLE, params);
    double time_tasks = experiment(tasks, SAMPLE, params);
    printf("time = %f (pthreads), %f (openmp tasks)\n",
           time_pthreads,
           time_tasks);
    // estimate speedup
    struct parameters seq_params = {params.start,
                                    params.finish,
                                    params.precision,
                                    1};
    double speedup_pthreads =
        speedup(experiment(pthreads, SAMPLE, seq_params), time_pthreads);
    double speedup_tasks =
        speedup(experiment(tasks, SAMPLE, seq_params), time_tasks);
    printf("speedup = %f (pthreads), %f (openmp tasks)\n",
           speedup_pthreads,
           speedup_tasks);
    // TODO:
    // here put your commentaries about the achieved speedup and execution time
    // (you may additionally provide parameters of the machine used to run the
    // experiment)
    // $ lscpu
    // Architecture:            x86_64
    // CPU op-mode(s):        32-bit, 64-bit
    //  Address sizes:         39 bits physical, 48 bits virtual
    //  Byte Order:            Little Endian
    // CPU(s):                  4
    //  On-line CPU(s) list:   0-3
    // Vendor ID:               GenuineIntel
    //  Model name:            Intel(R) Core(TM) i5-6300U CPU @ 2.40GHz
    //
    //
    //  RUNS:
    //  1.
    //  $ ./a.out
    //  lower bound = 0.000000
    //  upper bound = 1.000000
    //  precision = 0.000001
    //  threads = 1
    //  area = 0.341345, 0.341345
    //  time = 0.029580 (pthreads), 0.013835 (openmp tasks)
    //  speedup = 1.041516 (pthreads), 0.992368 (openmp tasks)
    //
    //  2.
    //  $ ./a.out 0 1 0.000001 4
    //  lower bound = 0.000000
    //  upper bound = 1.000000
    //  precision = 0.000001
    //  threads = 4
    //  area = 0.341345, 0.341345
    //  time = 0.017414 (pthreads), 0.013709 (openmp tasks)
    //  speedup = 1.700906 (pthreads), 0.958911 (openmp tasks)
    //
    //  3. ./a.out 0 1 0.001 3
    //  lower bound = 0.000000
    //  upper bound = 1.000000
    //  precision = 0.001000
    //  threads = 3
    //  area = 0.341970, 0.341345
    //  time = 0.000519 (pthreads), 0.000065 (openmp tasks)
    //  speedup = 0.159813 (pthreads), 0.250366 (openmp tasks)
    //
    //  THOUGHTS:
    //  OpenMP for once seems to be optimally spawning threads without creating too much
    //  additional overhead. I tested it with a variety of parameters showing 3 example
    //  runs above using 1, 4 and 3 threads with 4 producing the most amount of "speedup"

    return 0;
}
