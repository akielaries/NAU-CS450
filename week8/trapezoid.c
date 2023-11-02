#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <pthread.h>
#include <sys/time.h>
#include <unistd.h>

#define PI (3.141592653589793)

struct arguments
{
    double start;
    double finish;
    double precision;
    double* area;
};

double f(double x)
{
    return 1 / (sqrt(2 * PI)) * exp(-0.5 * x * x);
}

double trapeze_area(double a, double b, double h)
{
    return 0.5 * (a + b) * h;
}

void* calc(void* arg)
{
    struct arguments *argument;
    double start, finish, precision;
    double *area;
 
    argument = (struct arguments*)arg;
  
    start = argument->start;
    finish = argument->finish;
    precision = argument->precision;
    area = argument->area;
  
    *area = 0;

    for (double i = start; i < finish; i += precision)
        *area += trapeze_area(f(i), f(i + precision), precision);

    return NULL;
}

double calc_area(double x0, double x1, double dx, int nthreads)
{
    double area = 0;
    double areas[nthreads];
    pthread_t thread[nthreads];

    double width = (x1 - x0) / nthreads;

    // set arguments for threads
    struct arguments* args[nthreads];
    for (int t = 0; t < nthreads; t++)
    {
        args[t] = (struct arguments *)calloc(1, sizeof(struct arguments));
        args[t]->start = x0 + width * t; 
        args[t]->finish = x0 + width * (t + 1);
        args[t]->precision = dx;
        args[t]->area = &(areas[t]);
    }

    // create threads
    for (int t = 0; t < nthreads; t++)
    {
        pthread_create(&thread[t], NULL, calc, (void *)args[t]);
    }
    
    // join threads
    for (int t = 0; t < nthreads; t++)
    {
        pthread_join(thread[t], NULL);
    }
    
    // calculate total area
    for (int t = 0; t < nthreads; t++)
    {
        area += *args[t]->area;
    }

    return area;
}

int main(int argc, char* argv[])
{   
    struct timeval t;
    float t0, t1, st, pt;
    
    gettimeofday(&t, NULL);
    t0 = 0.001 * t.tv_sec + t.tv_usec;
    
    // default values of arguments:
    // 1) range (lower and upper bounds)
    double x0 = 0, x1 = 1;
    // 2-3) precision
    double prec = 1e-6;
    // 4) number of threads
    int nthreads = 3;

    // read arguments from console
    // assume that correct values and correct number of arguments were provided
    if (argc > 1)
    {
        x0 = atof(argv[1]);
        x1 = atof(argv[2]);
        prec = atof(argv[3]);
        nthreads = atoi(argv[4]);
    }

    // printf("lower bound = %f\n", x0);
    // printf("upper bound = %f\n", x1);
    // printf("precision = %0.9f\n", prec);
    // printf("threads = %d\n", nthreads);

    // sequential operations is a constant value
    usleep(1000);
    // sequential operations is a function of n
    // usleep((long)(0.01 * (x1 - x0) / prec));

    gettimeofday(&t, NULL);
    t1 = 0.001 * t.tv_sec + t.tv_usec;
    st = t1 - t0;
    // printf("sequential portion = %f\n", st);
   
    double area = calc_area(x0, x1, prec, nthreads);

    gettimeofday(&t, NULL);
    pt = 0.001 * t.tv_sec + t.tv_usec - t1;
    // printf("parallel portion = %f\n", pt);

    // printf("area = %f\n", area);

    printf("%f %f", st, pt);

    return 0;
}