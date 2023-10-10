#include <unistd.h>
#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <time.h>

#define N 5
#define ITER 1000
#define SPEED 100

pthread_mutex_t mutex;

// resources
int forks[N];
// total sleep time
int sleeps[N];

// prototypes
void *philosopher(void *arg);

void think(int id);
void pickupforks(int id);
void eat(int id);
void putdownforks(int id);

int waiter(int id);

int left(int id);
int right(int id);

int main() {
    srand(time(NULL));
    int i = 0;
    pthread_t thread[N];

    for (i = 0; i < N; i++) {
      // initially all forks available
      forks[i] = 1;
      // initial values for waiting time
      sleeps[i] = 0;
    }
    // initialize waiter lock
    pthread_mutex_init(&mutex, NULL);
    // create philosopher threads
    for (i = 0; i < N; i++) {
        int* idx = malloc(sizeof(int));
        *idx = i;
        pthread_create(&thread[i], NULL, philosopher, (void *)idx);
    }
    // join with the threads 
    for (i = 0; i < N; i++) {
        pthread_join(thread[i], NULL);
    }
    // show waiting time
    for (i = 0; i < N; i++) {
        printf("\nPhilosopher: %d Number of sleeps: %d", i, sleeps[i]);
    }
    pthread_mutex_destroy(&mutex);
    return 0; 
}

void *philosopher(void *arg) {
    int id = *(int*)arg;
    int count = 0;
    while (count < ITER) {
        think(id);
        pickupforks(id);
        eat(id);
        putdownforks(id);
        count++;
    }
    free(arg);
}

int left(int id) {
    return id;
}

int right(int id) {
    return (id + N - 1) % N;
}

int waiter(int id) {
    // returns 1 when the philosopher can eat
    int result = 0;

    pthread_mutex_lock(&mutex);
    if (forks[left(id)] && forks[right(id)]) {
        forks[left(id)] = 0;
        forks[right(id)] = 0;
        result = 1;
    }
    pthread_mutex_unlock(&mutex);

    return result;
}

// simulate thinking
void think(int id) {   
    printf("\nPhilosopher %d thinking", id);
    usleep((rand() % N + 1) * SPEED);
}

void pickupforks(int id) {
    while (!waiter(id)) {
        usleep(SPEED);
        sleeps[id]++;
    }
}

// simulate eat
void eat(int id) {
    printf("\nPhilosopher %d eating", id);
    usleep((rand() % N + 1) * SPEED);
}

void putdownforks(int id) {
    // return forks to waiter
    pthread_mutex_lock(&mutex);
    forks[left(id)] = 1;
    forks[right(id)] = 1;
    pthread_mutex_unlock(&mutex);
}
