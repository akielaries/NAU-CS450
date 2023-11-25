#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <math.h>
#include <omp.h>

double norm(double m, double s)
{
    double n01 = -6;
    for (int i = 0; i < 12; i++)
    {
        n01 += (double)rand() / RAND_MAX;
    }
    return m + n01 * s;
}

double mean(double* sample, long size)
{
    double res = 0;
    for (long i = 0; i < size; i++)
    {
        res += *(sample + i);
    }
    return res /= size;
}

double min(double* sample, long size)
{
    double res = *sample;
    for (long i = 1; i < size; i++)
    {
        if (*(sample + i) < res)
        {
            res = *(sample + i);
        } 
    }
    return res;
}

double max(double* sample, long size)
{
    double res = *sample;
    for (long i = 1; i < size; i++)
    {
        if (*(sample + i) > res)
        {
            res = *(sample + i);
        } 
    }
    return res;
}

double std(double* sample, long size)
{
    double res = 0;
    double avg = mean(sample, size);
    for (long i = 0; i < size; i++)
    {
        res += pow(*(sample + i) - avg, 2);
    }
    res /= size;
    return sqrt(res);
}

long* freq(double* sample, long size, int bins)
{
    long i;
    int j;
    long* fs = malloc(sizeof(long) * bins);
    double* bounds = malloc(sizeof(double) * (bins + 1));

    // the smallest and the biggest values in the sample
    double min_x = min(sample, size);
    double max_x = max(sample, size);
    // the bins' width
    double step = (max_x - min_x) / bins;

    // calculate bins' bounds
    for (j = 0; j < bins + 1; j++)
    {
        bounds[j] = min_x + step * j;
        // printf("b[%d] = %f\n", j, bounds[j]);
    }

    // initialize frequencies
    for (j = 0; j < bins; j++)
    {
        fs[j] = 0;
    }

    // calculate empirical frequencies
    for (i = 0; i < size; i++)
    {
        // check minimum sample values
        if (sample[i] == bounds[0])
        {
            fs[0]++;
            continue;
        }
        // check if out of bounds (due to floating point calculations)
        if (sample[i] > bounds[bins])
        {
            fs[bins - 1]++;
            continue;
        }
        // find the bin and increase the frequency
        for (j = 0; j < bins; j++)
        {
            if (sample[i] > bounds[j] && sample[i] <= bounds[j + 1])
            {
                fs[j]++;
                break;
            }
        }
    }
    
    return fs;
}

int main()
{
    double m = 10, s = 3;
    long size = 1e7;
    int bins = 10;
    
    double* sample = malloc(sizeof(double) * size);
    double* normalized = malloc(sizeof(double) * size);
    double s_avg, s_sd, n_avg, n_sd;

    //omp_set_num_threads(2);

    // generate a sample
    srand(time(NULL));

    clock_t start_time, end_time;

    start_time = clock();

    for (long i = 0; i < size; i++)
    {
        sample[i] = norm(m, s);
    }
    
    // calculate sample parameters
    s_avg = mean(sample, size);
    s_sd = std(sample, size);
    printf("Sample parameters: %.3f, %.3f\n", s_avg, s_sd);

    // normalize a sample
    for (long i = 0; i < size; i++)
    {
        normalized[i] = (sample[i] - s_avg) / s_sd;
    }
    
    // calculate parameters of the normalized sample
    n_avg = mean(normalized, size);
    n_sd = std(normalized, size);
    printf("Parameters of the normalized sample: %.3f, %.3f\n", n_avg, n_sd);

    // calculate empirical frequencies for the normalized sample
    long* fs = freq(normalized, size, bins);

    end_time = clock();

    double cpu_time_used = ((double)(end_time - start_time)) / CLOCKS_PER_SEC;

    // print out the results
    long sum_fs = 0;
    for (int i = 0; i < bins; i++)
    {
        printf("freq[%d] = %ld\n", i + 1, fs[i]);
        sum_fs += fs[i];
    }
    printf("Sum of frequencies = %ld (should be equal to the sample size!)\n", sum_fs);

    printf("Execution Time: %f seconds\n", cpu_time_used);
    //free(sample);
    //free(normalized);
    //free(fs);

    // there should be one more free i think

    return 0;
}
