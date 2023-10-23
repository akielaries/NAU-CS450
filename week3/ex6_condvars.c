#include <errno.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

pthread_mutex_t mutex_fuel;
pthread_cond_t cond_fuel;
int fuel = 0;

void *fuel_filling(void *arg) {
    for (int i = 0; i < 10; i++) {

        pthread_mutex_lock(&mutex_fuel);
        fuel++;
        printf("Filled fuel... %d\n", fuel);
        pthread_mutex_unlock(&mutex_fuel);

        pthread_cond_signal(&cond_fuel);
        sleep(1);
    }
}

void *car(void *arg) {
    pthread_mutex_lock(&mutex_fuel);
    while (fuel < 7) {
        printf("No enough fuel. Waiting...\n");
        pthread_cond_wait(&cond_fuel, &mutex_fuel);
    }
    fuel -= 7;
    printf("Fuel tanked. Now left: %d\n", fuel);
    pthread_mutex_unlock(&mutex_fuel);
}

int main(int argc, char *argv[]) {
    pthread_t fuel_thread, car_thread;

    pthread_mutex_init(&mutex_fuel, NULL);
    pthread_cond_init(&cond_fuel, NULL);

    // create threads
    if (pthread_create(&fuel_thread, NULL, &fuel_filling, NULL) != 0) {
        perror("Failed to create fuel thread");
    }
    if (pthread_create(&car_thread, NULL, &car, NULL) != 0) {
        perror("Failed to create car thread");
    }

    // join threads
    if (pthread_join(fuel_thread, NULL) != 0) {
        perror("Failed to join fuel thread");
    }
    if (pthread_join(car_thread, NULL) != 0) {
        perror("Failed to join car thread");
    }

    pthread_mutex_destroy(&mutex_fuel);
    pthread_cond_destroy(&cond_fuel);

    return 0;
}