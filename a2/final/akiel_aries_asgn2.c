/**
 * After talking with some students it seems we all interpreted this assignment
 * differently and agreed there was a lack of instruction, as always. It would
 * be nice for these remaing ~7 or so weeks for some sort of salvage to take
 * place where we can at least have clearly defined instructions for these
 * assignments and something to carry along with us after this class. So far,
 * nothing of real world value has been taught or learned and has 100% been
 * a regurgitation of pthreads documentation knowledge and arbitrary assignments
 */
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

// represents the number of dispatchers N and requests K to spread accross
// the dispatchers
int N, K;
// number of completed requests, blocked requests, and busy threads
int serviced_requests, blocked_requests, busy_threads = 0;
// global pthread mutex INIT variable
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

void *requests(void *arg);
void *dispatcher(void *arg);

/**
 * @brief This function I am using to simulate the process of a request being
 * processed. In this function, a dispatcher thread will operate on this
 * function as it sleeps for a random duration according to the instructions in
 * the slideshow.
 *
 * @param arg a pointer to the created pthread_ struct object
 */
void *requests(void *arg) {
    // current thread ID
    int thread_id = *((int *)arg);
    // sleep for a random amount of time to simulate request times
    int sleep_time = rand() % 5;
    printf("Service %d time = %d sec\n", thread_id, sleep_time);
    sleep(sleep_time);

    // lock current thread
    pthread_mutex_lock(&mutex);
    // if the amount of completed and denied requests is less than the total
    // keep going
    if (serviced_requests + blocked_requests < K) {
        // a request has not been serviced and is available for a dispatcher
        if (serviced_requests < K) {
            // increment number of completed requests
            serviced_requests++;
            printf("Thread %d serviced a request. Total complete: %d\n",
                   thread_id,
                   serviced_requests);
        }
    }
    // unlock thread
    pthread_mutex_unlock(&mutex);
    // EXIT CURRENT THREAD
    pthread_exit(NULL);
}

/**
 * @brief This function is used to simualte the process of dispatching
 * requests. Here is where we want to check the availability of each dispatcher
 * or designated thread in this case to determine to serve another request or
 * hault!
 *
 * param arg a pointer to the created pthread_ struct object
 */
void *dispatcher(void *arg) {
    // this is the id of the current thread found from the arg param passed in
    int thread_id = *((int *)arg);

    // this is an infinite loop that ends on the condition of no more requests
    // being available
    while (1) {
        // immediately lock this thread so no other requests can be ran on this
        // dispatcher
        pthread_mutex_lock(&mutex);
        // as long as the number of completed & declined requests is less than
        // the number of requests we can assume there are more requests to be
        // processed/dispatched, if not we assume there are no more request
        if (serviced_requests + blocked_requests >= K) {
            // release the lock on this thread as it is free now
            pthread_mutex_unlock(&mutex);
            // break out of the while loop
            break;
        }

        // more requests to be processed
        // else {
        // if the number of busy threads is greater than the dispatchers
        if (busy_threads < N) {
            // unlock mutex, freeing shared access to current thread
            pthread_mutex_unlock(&mutex);

            // simulate the business of the dispatcher with a random seed
            // between 0-2
            if (rand() % 2 == 0) {
                printf("Thread %d is currently working\n", thread_id);
                // lock on current thread
                pthread_mutex_lock(&mutex);
                // increment the number of "busy threads"
                busy_threads++;
                // unlock
                pthread_mutex_unlock(&mutex);
            }

            // dispatcher not busy
            else {
                // lock current thread
                pthread_mutex_lock(&mutex);

                // if there are still requests left, which is the assumption
                // of this block
                if (serviced_requests + blocked_requests < K) {
                    // unlock current thread
                    pthread_mutex_unlock(&mutex);
                    printf("Thread %d is waiting for a request\n", thread_id);
                    // sleep for a random number of time
                    int sleep_time = rand() % 3;
                    sleep(sleep_time);
                    // generate a random request ID for the current
                    // thread/dispatcher to process
                    int request_thread_id = rand() % N;
                    // if the randomly chosen request  ID is the thread_id,
                    // call the function
                    if (request_thread_id == thread_id) {
                        // call the requests function
                        requests(&thread_id);
                    }
                }
                // no more requests left!!
                else {
                    // final unlock on current thread
                    pthread_mutex_unlock(&mutex);
                    break; // no more requests to service
                }
            }
        }
        // if the dispatcher is busy decline/block the request!
        else {
            // unlock thread
            pthread_mutex_unlock(&mutex);
            printf("Thread %d is busy, request blocked!!\n", thread_id);
            // incriment number of blocked requests
            blocked_requests++;
            break;
        }
    }
    // EXIT CURRENT THREAD
    pthread_exit(NULL);
}

/**
 * @brief I think this function is supposed to simulate the processing of
 * dispatching and servicing requests so I used this to create and join threads
 * related to the above request and dispatch functions. We calculate the
 * "service level" based in the number of requests directly dispatched and
 * processed and taking into the account requests that had to wait for an
 * available dispatcher
 *
 * @param N dispatchers
 * @param K requests to serve
 * @return service_level the calculate number of requests directly processed
 */
double simulate(int N, int K) {
    // initialize a random seed
    srand(time(NULL));
    // create pthread type struct for K number of thread
    pthread_t requests_threads[K];
    // corresponding thread id's for each thread
    int request_thread_ids[K];

    // for every number of thread
    for (int i = 0; i < K; i++) {
        // attach corresponding ID
        request_thread_ids[i] = i;
        // create threads using the requests() function as the main call
        pthread_create(&requests_threads[i],
                       NULL,
                       requests,
                       &request_thread_ids[i]);
    }

    // then create dispatcher threads
    pthread_t dispatchers_threads[N];
    // dispatcher thread IDs
    int dispatcher_thread_ids[N];

    // for every thread
    for (int i = 0; i < N; i++) {
        // dispatcher thread ID
        dispatcher_thread_ids[i] = i;
        // create threads for dispatchers() function as main call
        pthread_create(&dispatchers_threads[i],
                       NULL,
                       dispatcher,
                       &dispatcher_thread_ids[i]);
    }

    // iterate over threads
    for (int i = 0; i < K; i++) {
        // join created threads for requests
        pthread_join(requests_threads[i], NULL);
    }
    // iterate over threads
    for (int i = 0; i < N; i++) {
        // join created threads for dispatchers
        pthread_join(dispatchers_threads[i], NULL);
    }

    // service level determined by number of service requests and taking into
    // account blocked/busy/declined dispatches. calculated based on the global
    // serviced_requests (completed) minus blocked_requests (blocked) divided
    // by the total number of specified requests
    double service_level = serviced_requests - blocked_requests / K;
    return service_level;
}

int main(int argc, char *argv[]) {
    // default values of arguments:
    // 1) number of dispatchers
    N = 1;
    // 2) number of requests to serve
    K = 10;

    // read arguments from console
    // assume that correct values and correct number of arguments were provided
    if (argc > 1) {
        N = atoi(argv[1]);
        K = atoi(argv[2]);
    }

    printf("Number of dispatchers = %d\n", N);
    printf("Demand size = %d\n", K);

    double service_level = simulate(N, K);
    printf("Service level = %f\n", service_level);

    return 0;
}
