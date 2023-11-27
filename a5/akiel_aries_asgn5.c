/** Akiel Aries aba275
 *
 * This assignment is pretty much identical to the previous in terms of the task
 * at hand so I once again am not entirely sure how we can speed this up
 * significantly in order to outweigh the overhead that comes with spawning
 * thread with openMP.
 *
 * - When running the program initially, with only the template code
 *   given to us that declare 2 threads with omp_set_num_threads, I got an
 *   execution time of about 4.1 seconds and after many attempts and some
 *   trial/error with threading certain loops and not others, I was only
 *   able to get the same execution time...
 *
 * - the main issue I ran into is none of my execution times differed
 *   enough to determine if speedup was ultimately correct. For example
 *   only theading the loops that are iterating the number of times as
 *   "size" = 1e7 (10000000), my execution time would sometimes dip
 *   to below 4.01 seconds but other times would spike up to 4.4, just
 *   like the sequential version we were given.
 *
 *   - I started my timers just after we declare the number of threads
 *   so that we do not include the time taken for spawning threads
 *   and we only calculate the time taken over things we have control
 *   over in our program, our functions.
 *
 *   - Since compilers are able to recognize patters, I am thinking
 *   since I am feeding it the same code over and over again it's able
 *   to recognize what to optimize itself even without flags the execution
 *   time for the sequential version and this "multithreaded" one are
 *   nearly identical
 *
 *
 *   - MY CODE'S EXECUTION:
 *   Sample parameters: 10.000, 3.000
 *   Parameters of the normalized sample: -0.000, 1.000
 *   freq[1] = 455
 *   freq[2] = 25336
 *   freq[3] = 334446
 *   freq[4] = 1616939
 *   freq[5] = 3343058
 *   freq[6] = 3123614
 *   freq[7] = 1311081
 *   freq[8] = 230551
 *   freq[9] = 14311
 *   freq[10] = 209
 *   Sum of frequencies = 10000000 (should be equal to the sample size!)
 *   Execution Time: 3.591127 seconds
 *
 *
 *   - THE GIVEN CODE'S EXECUTION:
 *   Sample parameters: 10.002, 3.000
 *   Parameters of the normalized sample: -0.000, 1.000
 *   freq[1] = 753
 *   freq[2] = 29229
 *   freq[3] = 340801
 *   freq[4] = 1553136
 *   freq[5] = 3192521
 *   freq[6] = 3114804
 *   freq[7] = 1443994
 *   freq[8] = 300199
 *   freq[9] = 24000
 *   freq[10] = 563
 *   Sum of frequencies = 10000000 (should be equal to the sample size!)
 *   Execution Time: 4.113517 seconds
 *
 *   - I ultimately decided what loops to parallize based on trial and error
 *   and also which onces would iterate the amount of times of size instead
 *   of bins since the former (size) is larger. I also chose based on which
 *   ones to not contain a branch within (if/else/conditional) which I believe
 *   helped in some minor way increase execution time...
 *
 *   - In the example template code to edit, there were memory leaks from
 *   allocations not having a corresponding deallocate so I added `free()`'s
 *   at the end of the program
 */
#include <math.h>
#include <omp.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

double norm(double m, double s) {
    double n01 = -6;
    for (int i = 0; i < 12; i++) {
        n01 += (double)rand() / RAND_MAX;
    }
    return m + n01 * s;
}

double mean(double *sample, long size) {
    double res = 0;
#pragma omp parallel for reduction(+ : res)
    for (long i = 0; i < size; i++) {
        res += *(sample + i);
    }
    return res /= size;
}

double min(double *sample, long size) {
    double res = *sample;
    for (long i = 1; i < size; i++) {
        if (*(sample + i) < res) {
            res = *(sample + i);
        }
    }
    return res;
}

double max(double *sample, long size) {
    double res = *sample;
    for (long i = 1; i < size; i++) {
        if (*(sample + i) > res) {
            res = *(sample + i);
        }
    }
    return res;
}

double std(double *sample, long size) {
    double res = 0;
    double avg = mean(sample, size);
#pragma omp parallel for reduction(+ : res)
    for (long i = 0; i < size; i++) {
        res += pow(*(sample + i) - avg, 2);
    }
    res /= size;
    return sqrt(res);
}

long *freq(double *sample, long size, int bins) {
    long i;
    int j;
    long *fs = malloc(sizeof(long) * bins);
    double *bounds = malloc(sizeof(double) * (bins + 1));

    // the smallest and the biggest values in the sample
    double min_x = min(sample, size);
    double max_x = max(sample, size);
    // the bins' width
    double step = (max_x - min_x) / bins;

    // calculate bins' bounds
    for (j = 0; j < bins + 1; j++) {
        bounds[j] = min_x + step * j;
    }

    // initialize frequencies
    for (j = 0; j < bins; j++) {
        fs[j] = 0;
    }

    // this code isn't really multithreading friendly as it contains
    // many branches
    // calculate empirical frequencies
    for (i = 0; i < size; i++) {
        // check minimum sample values
        if (sample[i] == bounds[0]) {
            fs[0]++;
            continue;
        }
        // check if out of bounds (due to floating point calculations)
        if (sample[i] > bounds[bins]) {
            fs[bins - 1]++;
            continue;
        }
        // find the bin and increase the frequency
        for (j = 0; j < bins; j++) {
            if (sample[i] > bounds[j] && sample[i] <= bounds[j + 1]) {
                fs[j]++;
                break;
            }
        }
    }

    free(bounds);

    return fs;
}

int main() {
    double m = 10, s = 3;
    long size = 1e7;
    int bins = 10;

    double *sample = (double *)malloc(sizeof(double) * size);
    double *normalized = (double *)malloc(sizeof(double) * size);
    double s_avg, s_sd, n_avg, n_sd;

    omp_set_num_threads(2);

    // generate a sample
    srand(time(NULL));

    // clock_t start_time, end_time;

    // START CLOCK HERE, START OF OUR FUNCTION CALLS
    // start_time = clock();
    double start_time = omp_get_wtime();

    // even tho this iterates `size` times, it is still not distributed
    // accross threads in a friendly manner and increases execution time
    // by 10
    for (long i = 0; i < size; i++) {
        sample[i] = norm(m, s);
    }

    // calculate sample parameters
    s_avg = mean(sample, size);
    s_sd = std(sample, size);
    printf("Sample parameters: %.3f, %.3f\n", s_avg, s_sd);

    // normalize a sample
    // parallelizing this does nothing but increase execution time
    for (long i = 0; i < size; i++) {
        normalized[i] = (sample[i] - s_avg) / s_sd;
    }

    // calculate parameters of the normalized sample
    n_avg = mean(normalized, size);
    n_sd = std(normalized, size);
    printf("Parameters of the normalized sample: %.3f, %.3f\n", n_avg, n_sd);

    // calculate empirical frequencies for the normalized sample
    long *fs = freq(normalized, size, bins);

    // STOP CLOCK HERE, LAST OF OUR FUNCTION CALLS
    // end_time = clock();
    // double cpu_time_used = ((double)(end_time - start_time)) /
    // CLOCKS_PER_SEC;
    double total_time = omp_get_wtime() - start_time;

    // print out the results
    long sum_fs = 0;
    for (int i = 0; i < bins; i++) {
        printf("freq[%d] = %ld\n", i + 1, fs[i]);
        sum_fs += fs[i];
    }
    printf("Sum of frequencies = %ld (should be equal to the sample size!)\n",
           sum_fs);

    printf("Execution Time: %f seconds\n", total_time);

    free(sample);
    free(normalized);
    free(fs);

    return 0;
}
