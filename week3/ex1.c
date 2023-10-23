#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

void *func(void *);

int main() {
    pthread_t thread1, thread2;

    pthread_create(&thread1, NULL, func, NULL);
    pthread_create(&thread2, NULL, func, NULL);

    pthread_join(thread1, NULL);
    pthread_join(thread2, NULL);

    printf("Main runs...\n");
    return 0;
}

void *func(void *) {
    sleep(1);
    printf("Thread runs...%d \n", pthread_self());
}
