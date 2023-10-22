#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

int N;
int K;
int serviced_requests = 0;
int declined_requests = 0;
int busy_threads = 0;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

void* request_routine(void* arg) {
    int thread_id = *((int*)arg);
    int sleep_time = rand() % 5;
    printf("Service %d time = %d sec\n", thread_id, sleep_time);
    sleep(sleep_time);

    pthread_mutex_lock(&mutex);
    if (serviced_requests + declined_requests < K) {
        if (serviced_requests < K) {
            serviced_requests++;
            printf("Thread %d serviced a request. Total serviced: %d\n", thread_id, serviced_requests);
        } else {
            declined_requests++;
            printf("Thread %d declined a request. Total declined: %d\n", thread_id, declined_requests);
        }
    }
    pthread_mutex_unlock(&mutex);
    pthread_exit(NULL);
}

void* dispatcher_routine(void* arg) {
    int thread_id = *((int*)arg);
    while (serviced_requests + declined_requests < K) {
        if (rand() % 2 == 0) {
            printf("Thread %d is busy.\n", thread_id);
            busy_threads++;
        } else {
            pthread_mutex_lock(&mutex);
            if (serviced_requests + declined_requests < K) {
                pthread_mutex_unlock(&mutex);
                printf("Thread %d is waiting for a request.\n", thread_id);
                int sleep_time = rand() % 3;
                sleep(sleep_time);
                int request_thread_id = rand() % N;
                if (request_thread_id == thread_id) {
                    pthread_t request_thread;
                    pthread_create(&request_thread, NULL, request_routine, &thread_id);
                    pthread_join(request_thread, NULL);
                }
            } else {
                pthread_mutex_unlock(&mutex);
                break; // No more requests to service
            }
        }
    }
    pthread_exit(NULL);
}

double simulate(int N, int K) {
    srand(time(NULL));

    pthread_t dispatchers[N];
    int thread_ids[N];

    for (int i = 0; i < N; i++) {
        thread_ids[i] = i;
        pthread_create(&dispatchers[i], NULL, dispatcher_routine, &thread_ids[i]);
    }

    for (int i = 0; i < N; i++) {
        pthread_join(dispatchers[i], NULL);
    }

    double service_level = (double)serviced_requests / (serviced_requests + declined_requests + busy_threads);
    return service_level;
}

int main(int argc, char* argv[]) {
    N = 1;
    K = 10;

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

