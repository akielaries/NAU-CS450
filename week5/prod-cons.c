#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <time.h>
#include <semaphore.h>

#define PRODUCERS 2
#define CONSUMERS 8
#define BUFFER 10

sem_t sem_empty;
sem_t sem_full;

pthread_mutex_t mutex;

int buffer[BUFFER];
int count = 0;

void* producer(void* arg) {
    int* idx = (int*)arg;

    while (1) {
        // produce
        int x = rand() % 100;
        printf("%d produced by %d\n", x, *idx);
        usleep(50000);

        // add to the buffer
        sem_wait(&sem_empty);
        
        pthread_mutex_lock(&mutex);
        buffer[count] = x;
        count++;
        pthread_mutex_unlock(&mutex);
        
        sem_post(&sem_full);
    }

    free(arg);
}

void* consumer(void* arg) {
    int* idx = (int*)arg;

    while (1) {
        int y;
        // remove from the buffer
        sem_wait(&sem_full);
        
        pthread_mutex_lock(&mutex);
        y = buffer[count - 1];
        count--;
        pthread_mutex_unlock(&mutex);
        
        sem_post(&sem_empty);

        // consume
        printf("%d consumed by %d\n", y, *idx);
        sleep(1);
    }
    free(arg);
}

int main() {
    srand(time(NULL));
    int i;
    
    pthread_t th_consumer[CONSUMERS];
    pthread_t th_producer[PRODUCERS];
    
    pthread_mutex_init(&mutex, NULL);
    sem_init(&sem_empty, 0, BUFFER);
    sem_init(&sem_full, 0, 0);

    // create threads
    for (i = 0; i < CONSUMERS; i++) {
        int* idx = malloc(sizeof(int));
        *idx = i;
        pthread_create(&th_consumer[i], NULL, &consumer, (void*) idx);
    }
    for (i = 0; i < PRODUCERS; i++) {
        int* idx = malloc(sizeof(int));
        *idx = i;
        pthread_create(&th_producer[i], NULL, &producer, (void*) idx);
    }
    // join threads
    for (i = 0; i < CONSUMERS; i++) {
        pthread_join(th_consumer[i], NULL);
    }
    for (i = 0; i < PRODUCERS; i++) {
        pthread_join(th_producer[i], NULL);
    }

    sem_destroy(&sem_empty);
    sem_destroy(&sem_full);
    pthread_mutex_destroy(&mutex);

    return 0;
}