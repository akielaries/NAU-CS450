#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <omp.h>

#define THREADS 4

int main()
{   
    // the number of points
    long points = 1e8;

    omp_set_num_threads(THREADS);
    
    long in_circle = 0;

    #pragma omp parallel reduction(+: in_circle)
    {
        srand(time(NULL) + omp_get_thread_num());
        
        #pragma omp for
        for (int i = 0; i < points; i++)
        {
            double x = (double)(rand()) / RAND_MAX;
            double y = (double)(rand()) / RAND_MAX;
            
            in_circle += x * x + y * y <= 1;
            // if (x * x + y * y <= 1) in_circle++;
        }
    }
    

    double pi = 4 * (double)in_circle / points;
    printf("estimated PI = %f\n", pi);

    return 0;
}