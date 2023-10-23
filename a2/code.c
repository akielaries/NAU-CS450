#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

// Mutex to protect shared resources
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

// Global variables
int N; // Number of dispatchers
int K; // Number of requests to serve
int requests_served = 0;
int requests_declined = 0;

// Function to simulate request handling by a dispatcher
void *dispatcher_routine(void *arg) {
    int dispatcher_id = *(int *)arg;

    while (1) {
        // Simulate random request arrival
        sleep(rand() % 3);

        if (requests_served + requests_declined < K) {
            // Check if there's an available dispatcher
            if (pthread_mutex_trylock(&mutex) == 0) {
                if (requests_served + requests_declined < K) {
                    requests_served++;
                    printf("Dispatcher %d is serving request %d\n",
                           dispatcher_id,
                           requests_served);
                    pthread_mutex_unlock(&mutex);
                    // Simulate random servicing time
                    sleep(rand() % 3);
                } else {
                    pthread_mutex_unlock(&mutex);
                    break; // No more requests to serve
                }
            } else {
                requests_declined++;
                printf(
                    "Request declined by Dispatcher %d (Total Declined: %d)\n",
                    dispatcher_id,
                    requests_declined);
            }
        } else {
            break;
        }
    }

    pthread_exit(NULL);
}

double simulate(int N, int K) {
    pthread_t dispatchers[N];
    int dispatcher_ids[N];

    for (int i = 0; i < N; i++) {
        dispatcher_ids[i] = i + 1;
        pthread_create(&dispatchers[i],
                       NULL,
                       dispatcher_routine,
                       &dispatcher_ids[i]);
    }

    for (int i = 0; i < N; i++) {
        pthread_join(dispatchers[i], NULL);
    }

    return (double)requests_served / K;
}

int main(int argc, char *argv[]) {
    srand(time(NULL)); // Seed the random number generator

    // Default values of arguments:
    N = 1;  // Number of dispatchers
    K = 10; // Number of requests to serve

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
