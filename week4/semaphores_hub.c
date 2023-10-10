#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <pthread.h>
#include <semaphore.h>

#define DISPATCHERS 5
#define REQUESTS 10

sem_t semaphore;

void* serve(void* arg) {
    int idx = *(int*)arg;
    printf("Request %d: waiting\n", idx);

    sem_wait(&semaphore);
    int count;
    sem_getvalue(&semaphore, &count);

    printf("Request %d: servicing started\n", idx);
    sleep(rand() % 5 + 1);
    printf("Request %d: servicing finished\n", idx);
    
    sem_post(&semaphore);

    free(arg);
}

int main(int argc, char *argv[]) {
    srand(time(NULL));

    sem_init(&semaphore, 0, 1);
    pthread_t thread[REQUESTS];
    int i;

    for (i = 0; i < REQUESTS; i++) {
        int* idx = malloc(sizeof(int));
        *idx = i;
        pthread_create(&thread[i], NULL, &serve, (void *) idx);
    }

    for (i = 0; i < REQUESTS; i++) {
        pthread_join(thread[i], NULL);    
    }

    sem_destroy(&semaphore);

    return 0;
}