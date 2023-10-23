#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#define MAX_DISPATCHERS 10
#define MAX_REQUESTS 100

// Global variables and structures
int N, K;
int serviced_requests = 0;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

// Function to simulate servicing a request
void *request_routine(void *arg) {
    int thread_id = *((int *)arg);
    int sleep_time = rand() % 5; // Simulating random request time
    printf("Service %d time = %d sec\n", thread_id, sleep_time);
    sleep(sleep_time);
    pthread_mutex_lock(&mutex);
    printf("mutex lock on : %d \n", thread_id);

    if (serviced_requests < K) {
        serviced_requests++;
        printf("Thread %d serviced a request.\n", thread_id);
    }
    printf("mutex unlock on : %d \n", thread_id);
    pthread_mutex_unlock(&mutex);
    pthread_exit(NULL);
}

// Function to simulate the dispatcher routine
void *dispatcher_routine(void *arg) {
    int thread_id = *((int *)arg);
    while (serviced_requests < K) {
        if (rand() % 2 == 0) {
            printf("Thread %d is busy.\n", thread_id);
        } else {
            printf("Thread %d is waiting for a request.\n", thread_id);
            int sleep_time = rand() % 3; // Simulating random wait time
            sleep(sleep_time);
            int request_thread_id = rand() % N;
            if (request_thread_id == thread_id) {
                pthread_t request_thread;
                pthread_create(&request_thread,
                               NULL,
                               request_routine,
                               &thread_id);
                pthread_join(request_thread, NULL);
            }
        }
    }
    pthread_exit(NULL);
}

double simulate(int N, int K) {
    // Initialize random seed
    srand(time(NULL));

    // Create dispatcher threads
    pthread_t dispatchers[N];
    int thread_ids[N];
    for (int i = 0; i < N; i++) {
        thread_ids[i] = i;
        pthread_create(&dispatchers[i],
                       NULL,
                       dispatcher_routine,
                       &thread_ids[i]);
    }

    // Wait for dispatcher threads to finish
    for (int i = 0; i < N; i++) {
        pthread_join(dispatchers[i], NULL);
    }

    double service_level = (double)serviced_requests / K;
    return service_level;
}

int main(int argc, char *argv[]) {
    // Default values of arguments:
    // 1) number of dispatchers
    N = 1;
    // 2) number of requests to serve
    K = 10;

    // Read arguments from console
    // Assume that correct values and correct number of arguments were provided
    if (argc > 2) {
        N = atoi(argv[1]);
        K = atoi(argv[2]);
    }

    printf("Number of dispatchers = %d\n", N);
    printf("Demand size = %d\n", K);

    double service_level = simulate(N, K);
    printf("Service level = %f\n", service_level);

    return 0;
}
