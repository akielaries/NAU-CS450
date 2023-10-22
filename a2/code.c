#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

// Define the number of dispatchers and requests
int N;
int K;

// Mutex to protect shared data
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

// Global variables to keep track of the service level
int totalServiced = 0;
int totalRequests = 0;

// Dispatcher thread function
void* dispatcher(void* arg) {
    int dispatcher_id = *((int*)arg);

    while (1) {
        // Simulate random request arrival
        sleep(rand() % 5);

        pthread_mutex_lock(&mutex);
        if (totalRequests >= K) {
            pthread_mutex_unlock(&mutex);
            break; // All requests have been serviced
        }

        totalRequests++;

        // Check if dispatcher is available
        if (totalServiced < K) {
            // Simulate random service time
            sleep(rand() % 5);
            totalServiced++;

            printf("Dispatcher %d serviced a request. Total serviced: %d\n", dispatcher_id, totalServiced);
        }
        pthread_mutex_unlock(&mutex);
    }

    return NULL;
}

double simulate(int N, int K) {
    // Initialize random number generator
    srand(time(NULL));

    // Create dispatcher threads
    pthread_t dispatchers[N];
    int dispatcher_ids[N];

    for (int i = 0; i < N; i++) {
        dispatcher_ids[i] = i + 1;
        pthread_create(&dispatchers[i], NULL, dispatcher, &dispatcher_ids[i]);
    }

    // Wait for dispatcher threads to finish
    for (int i = 0; i < N; i++) {
        pthread_join(dispatchers[i], NULL);
    }

    return (double)totalServiced / K;
}

int main(int argc, char* argv[]) {
    // Default values of arguments
    N = 1;
    K = 10;

    // Read arguments from the command line
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

