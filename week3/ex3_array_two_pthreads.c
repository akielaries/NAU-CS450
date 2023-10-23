#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

#define SIZE 1000

struct arguments {
    int *array;
    int size;
    int *sum;
};

void *do_work(void *arg);

// This program has two threads compute the sum of an array
int main(int argc, char *argv) {
    int array[SIZE];
    int i, sum[2];
    int t;

    pthread_t worker_thread[2];
    struct arguments *arg[2];

    // Fill in array with integer values
    for (i = 0; i < SIZE; i++) {
        array[i] = 1 + i;
    }

    // Create argument for child thread #0
    arg[0] = (struct arguments *)calloc(1, sizeof(struct arguments));
    arg[0]->array = &(array[0]);
    arg[0]->size = SIZE / 2;
    arg[0]->sum = &(sum[0]);

    // Create argument for child thread #1
    arg[1] = (struct arguments *)calloc(1, sizeof(struct arguments));
    arg[1]->array = &(array[SIZE / 2]);
    arg[1]->size = SIZE / 2;
    arg[1]->sum = &(sum[1]);

    // Creating child threads
    for (t = 0; t < 2; t++) {
        if (pthread_create(&worker_thread[t], NULL, do_work, (void *)arg[t])) {
            fprintf(stderr, "Error while creating thread #%d\n", t);
            exit(1);
        }
        fprintf(stdout, "Waiting for child thread to complete\n");
    }

    // Joining with child threads
    for (t = 0; t < 2; t++) {
        if (pthread_join(worker_thread[t], NULL)) {
            fprintf(stderr, "Error while joining with child thread #%d\n", t);
            exit(1);
        }
    }

    // Printing the sum of the two partial sums
    fprintf(stdout, "Sum of the array: %d\n", sum[0] + sum[1]);
    fprintf(stdout, "(the result should be %d)\n", SIZE * (SIZE + 1) / 2);
    exit(0);
}

void *do_work(void *arg) {
    struct arguments *argument;
    int i, size;
    int *array;
    int *sum;

    argument = (struct arguments *)arg;

    size = argument->size;
    array = argument->array;
    sum = argument->sum;

    *sum = 0;
    for (i = 0; i < size; i++)
        *sum += array[i];

    return NULL;
}