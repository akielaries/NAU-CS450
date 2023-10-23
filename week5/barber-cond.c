// Barber shop problem: using locks and condition variables

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#define CUSTOMERS 10

// three mutual exclusion sections with a single mutex
pthread_mutex_t mutex;
// condition variables
pthread_cond_t cond_cust_left, cond_barber_idle, cond_door, cond_chair;

// 0 / 1
int cust_flag = 0;   // gone / still here
int barber_flag = 0; // idle / working
int door_flag = 1;   // closed / open
int chair_flag = 0;  // free / occupied

// prototypes
void *customer(void *arg);
void *barber(void *arg);
void dohaircut();
void custleave();

int main() {
    srand(time(NULL));
    int i;

    pthread_t barber_thread;
    pthread_t customer_thread[CUSTOMERS];

    pthread_cond_init(&cond_cust_left, NULL);
    pthread_cond_init(&cond_barber_idle, NULL);
    pthread_cond_init(&cond_door, NULL);
    pthread_cond_init(&cond_chair, NULL);

    // initialize locks
    pthread_mutex_init(&mutex, NULL);

    // create customer threads
    for (i = 0; i < CUSTOMERS; i++) {
        pthread_create(&customer_thread[i], NULL, customer, (void *)(long)i);
    }

    // create barber thread
    pthread_create(&barber_thread, NULL, barber, NULL);

    // join with the threads
    pthread_join(barber_thread, NULL);
    for (i = 0; i < CUSTOMERS; i++) {
        pthread_join(customer_thread[i], NULL);
    }

    // destroy the conditions and the mutex:
    // ...

    return 0;
}

void *customer(void *arg) {
    int id = (long)arg;

    // we give the customers random times to enter the barbershop
    // simulates constomers going to the shop at different times
    sleep(rand() % 9 + 1);

    printf("\n customer %d waiting for door to be open", id);

    pthread_mutex_lock(&mutex); // --->>> start critical section
    while (!door_flag) {
        pthread_cond_wait(&cond_door, &mutex);
    }
    // customer must shut the door
    door_flag = 0;
    printf("\n customer %d enters", id);

    // wake up the barber
    barber_flag = 1;
    // customer sits in chair, barber begins cutting hair
    chair_flag = 1;
    // customer is still here
    cust_flag = 1;

    // signal sitting in chair
    pthread_cond_signal(&cond_chair);
    printf("\n customer %d sits in chair", id);

    // wait for the barber to not be busy cutting hair, e.g., sleeping
    while (barber_flag) {
        pthread_cond_wait(&cond_barber_idle, &mutex);
    }

    printf("\n customer %d gets haircut", id);
    // customer gets out of the chair
    chair_flag = 0;

    // customer leaves
    custleave();
    printf("\n customer %d leaves", id);

    // customer left
    cust_flag = 0;
    pthread_cond_signal(&cond_cust_left);

    pthread_mutex_unlock(&mutex); // --->>> end critical section
}

void getcustomer() {
    // wait for the customer to sit in the chair
    pthread_mutex_lock(&mutex);
    while (!chair_flag) {
        pthread_cond_wait(&cond_chair, &mutex);
    }
    pthread_mutex_unlock(&mutex);
}

void finish() {
    // finish the haircut
    pthread_mutex_lock(&mutex);
    // after cutting hair, tell customer done working
    barber_flag = 0;
    pthread_cond_signal(&cond_barber_idle);

    // wait for customer to leave the shop
    while (cust_flag) {
        pthread_cond_wait(&cond_cust_left, &mutex);
    }

    // unlock the door for the next customer
    door_flag = 1;
    pthread_cond_signal(&cond_door);
    pthread_mutex_unlock(&mutex);
}

// barber:
// 1) waits for the customer to sit in the chair (getcustomer)
// 2) finishes the haircut
void *barber(void *arg) {
    int count = 0;

    while (count < CUSTOMERS) {
        count++;
        // count customers starting from 1
        printf("\n\nBarber Customer Served %d \n", count);
        // get next customer to sit in the chair
        getcustomer();
        // cut hair
        dohaircut();
        // finish the haircut
        finish();
    }

    printf("\n\nBarbershop closed for the day! %d customers served\n\n", count);
}

// simulate haircut time by sleeping
void dohaircut() {
    sleep(1);
}

// simulate leaving shop by sleeping
void custleave() {
    sleep(1);
}