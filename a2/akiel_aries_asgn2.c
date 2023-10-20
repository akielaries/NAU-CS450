#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <time.h>

#define MAX_DISPATCHERS 10
#define MAX_REQUESTS 100

// Global variables and structures
int N, K;
int serviced_requests = 0;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

// Function to simulate servicing a request
void* request_routine(void* arg) {
    int dispatcher_id = *((int*)arg);
    int sleep_time = rand() % 5;  // Simulating random request time
    sleep(sleep_time);
    pthread_mutex_lock(&mutex);
    if (serviced_requests < K) {
        serviced_requests++;
        printf("Dispatcher %d serviced a request.\n", dispatcher_id);
    }
    pthread_mutex_unlock(&mutex);
    pthread_exit(NULL);
}

// Function to simulate the dispatcher routine
void* dispatcher_routine(void* arg) {
    int dispatcher_id = *((int*)arg);
    while (serviced_requests < K) {
        if (rand() % 2 == 0) {
            printf("Dispatcher %d is busy.\n", dispatcher_id);
        } else {
            printf("Dispatcher %d is waiting for a request.\n", dispatcher_id);
            int sleep_time = rand() % 3;  // Simulating random wait time
            sleep(sleep_time);
            int request_dispatcher_id = rand() % N;
            if (request_dispatcher_id == dispatcher_id) {
                pthread_t request_thread;
                pthread_create(&request_thread, NULL, request_routine, &dispatcher_id);
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
    pthread_t dispatchers[MAX_DISPATCHERS];
    int dispatcher_ids[MAX_DISPATCHERS];
    for (int i = 0; i < N; i++) {
        dispatcher_ids[i] = i;
        pthread_create(&dispatchers[i], NULL, dispatcher_routine, &dispatcher_ids[i]);
    }

    // Wait for dispatcher threads to finish
    for (int i = 0; i < N; i++) {
        pthread_join(dispatchers[i], NULL);
    }

    double service_level = (double)serviced_requests / K;
    return service_level;
}

int main(int argc, char* argv[]) {
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

