#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
// other headers may be inclueded

// auxiliary global variables and functions may be declared here

// the thread function(s) should be defined here

double simulate(int N, int K) {
    // simulates the process of servicing the K requests by N dispatchers
    // and returns the service level which is a part of serviced (not declined)
    // requests in the total number of requests

    double level = 0;

    // put your code here

    return level;
}

int main(int argc, char *argv[]) {
    // default values of arguments:
    // 1) number of dispatchers
    int N = 1;
    // 2) number of requests to serve
    int K = 10;

    // read arguments from console
    // assume that correct values and correct number of arguments were provided
    if (argc > 1) {
        N = atoi(argv[0]);
        K = atoi(argv[1]);
    }

    printf("Number of dispatchers = %d\n", N);
    printf("Demand size = %d\n", K);

    double service_level = simulate(N, K);
    printf("Service level = %f\n", service_level);

    return 0;
}