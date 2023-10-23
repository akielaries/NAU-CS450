#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

#define N 100000
#define THREADS 10

pthread_mutex_t mutex;

int global = 0;

void *incr(void *arg) {
    for (size_t i = 0; i < N; i++) {
        pthread_mutex_lock(&mutex);
        global++;
        pthread_mutex_unlock(&mutex);
    }
    return NULL;
}

void *decr(void *arg) {
    for (size_t i = 0; i < N; i++) {
        pthread_mutex_lock(&mutex);
        global--;
        pthread_mutex_unlock(&mutex);
    }
    return NULL;
}

int main(int argc, char **argv) {
    pthread_t thread[THREADS];

    pthread_mutex_init(&mutex, NULL);

    for (int i = 0; i < THREADS; i++) {
        if (i % 2) {
            pthread_create(&thread[i], NULL, incr, NULL);
        } else {
            pthread_create(&thread[i], NULL, decr, NULL);
        }
    }

    for (int i = 0; i < THREADS; i++) {
        pthread_join(thread[i], NULL);
    }

    pthread_mutex_destroy(&mutex);

    fprintf(stdout, "Result = %d\n", global);
    return 0;
}