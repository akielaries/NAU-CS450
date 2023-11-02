#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <omp.h>

#define PI (3.141592653589793)

double f(double x)
{
    return 1 / (sqrt(2 * PI)) * exp(-0.5 * x * x);
}

double calc_area(double x0, double x1, long points)
{
    double area = 0;
    
    // put your code here
        
    return area;
}

int main(int argc, char* argv[])
{   
    // default values of arguments:
    // 1-2) range (lower and upper bounds)
    double x0 = 0, x1 = 1;
    // 3) the number of points
    long points = 1e8;
    // 4) the number of threads
    int threads = 1;

    // read arguments from console
    // assume that correct values and correct number of arguments were provided
    if (argc > 1)
    {
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
