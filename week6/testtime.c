#include <stdio.h>
#include <unistd.h>
#include <sys/time.h>

int main() {

    struct timeval t0, t1;

    gettimeofday(&t0, NULL);
    usleep(1e4);
    gettimeofday(&t1, NULL);

    float dt = (t1.tv_sec - t0.tv_sec) + (t1.tv_usec - t0.tv_usec) / 1e6;
    printf("time=%f\n", dt);

    return 0;
}