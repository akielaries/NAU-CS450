#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

struct arguments {
    int *array;
    int size;
    int *sum;
};

void *do_work(void *arg);

int main(int argc, char *argv) {
    int array[10];
    int i, sum;

    pthread_t worker_thread;

    struct arguments *arg;

    // build argument to threads
    arg = (struct arguments *)calloc(1, sizeof(struct arguments));
    arg->array = array;
    arg->size = 10;
    arg->sum = &sum;

    // fill in integer values into array
    for (i = 0; i < arg->size; i++) {
        array[i] = 1 + i;
    }

    // create thread
    if (pthread_create(&worker_thread, NULL, do_work, (void *)arg)) {
        fprintf(stderr, "Error while creating thread\n");
        exit(1);
    }

    // join with thread
    if (pthread_join(worker_thread, NULL)) {
        fprintf(stderr, "Error while joining with child thread\n");
        exit(1);
    }

    fprintf(stdout, "Sum of the array: %d\n", sum);
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
    for (i = 0; i < size; i++) {
        *sum += array[i];
    }

    return NULL;
}
