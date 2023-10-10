#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <time.h>

#define CYCLES 100
#define MAX_SHIPMENT 50
#define CAPACITY 10
#define VEHICLES 5

pthread_mutex_t mutex;
pthread_cond_t cond;

int parcels = 0;

void* incoming_flow() {

    for (int i = 0; i < CYCLES; i++)
    {
        int shipment = rand() % MAX_SHIPMENT + 1;
        
        pthread_mutex_lock(&mutex);

        parcels += shipment;
        printf("%d parcels shipped. Total parcels: %d\n", shipment, parcels);
        
        pthread_mutex_unlock(&mutex);
        pthread_cond_broadcast(&cond);

        sleep(1);
    }
    
    return NULL;
}

void* deliver() {

    pthread_mutex_lock(&mutex);

    while (parcels < CAPACITY) {
        printf("Not enough parcels. Waiting...\n");
        pthread_cond_wait(&cond, &mutex);
    }
    
    parcels -= CAPACITY;
    printf("Run servicing. Parcels left: %d\n", parcels);

    pthread_mutex_unlock(&mutex);

    return NULL;
}

int main() {
    srand(time(NULL));

    pthread_t th_hub;
    pthread_t th_vehicle[VEHICLES];
    int i;

    pthread_cond_init(&cond, NULL);
    pthread_mutex_init(&mutex, NULL);

    pthread_create(&th_hub, NULL, incoming_flow, NULL);
    for (i = 0; i < VEHICLES; i++)
    {
        pthread_create(&th_vehicle[i], NULL, deliver, NULL);
    }


    pthread_join(th_hub, NULL);
    for (i = 0; i < VEHICLES; i++)
    {
        pthread_join(th_vehicle[i], NULL);
    }

    pthread_mutex_destroy(&mutex);
    pthread_cond_destroy(&cond);

}