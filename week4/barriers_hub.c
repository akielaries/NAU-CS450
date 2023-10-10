#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>

#define DISTRICTS 5
#define CAPACITY 10

int selected[DISTRICTS], parcels[DISTRICTS];

pthread_barrier_t demand_barrier, selected_barrier;

void* demand(void* arg) {
    
    int idx = *(int*)arg;

    // generate demand
    parcels[idx] = rand() % CAPACITY + 1;

    pthread_barrier_wait(&demand_barrier);
    pthread_barrier_wait(&selected_barrier);

    if (selected[idx] == 1) {
        printf("Parcels %d in district %d selected\n", parcels[idx], idx);
    }
    else {
        printf("Parcels %d in district %d wait\n", parcels[idx], idx);
    }

    free(arg);
    return NULL;
}

int main() {
    
    srand(time(NULL));
    
    pthread_t th_district[DISTRICTS];
    int i;
    pthread_barrier_init(&demand_barrier, NULL, DISTRICTS + 1);
    pthread_barrier_init(&selected_barrier, NULL, DISTRICTS + 1);

    for (i = 0; i < DISTRICTS; i++) {
        int* index = malloc(sizeof(int));
        *index = i;
        pthread_create(&th_district[i], NULL, demand, (void *) index);
    }
   
    // select a district to serve
    pthread_barrier_wait(&demand_barrier);
    int max = 0, dselected = 0;
    for (i = 0; i < DISTRICTS; i++) {
        if (parcels[i] > max) {
            max = parcels[i];
            dselected = i;
        }
    }
    printf("max=%d, dselected=%d\n", max, dselected);
    selected[dselected] = 1;
    pthread_barrier_wait(&selected_barrier);
    
    for (i = 0; i < DISTRICTS; i++) {
        pthread_join(th_district[i], NULL);
    }    

    pthread_barrier_destroy(&demand_barrier);
    pthread_barrier_destroy(&selected_barrier);

    return 0;
}