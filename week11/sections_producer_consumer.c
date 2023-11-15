// Producer-Consumer Problem

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <omp.h>

#define SIZE 10

int main()
{
    int* buffer = malloc(sizeof(int) * SIZE);
    int count = 0;

    srand(time(NULL));

    #pragma omp parallel sections
    {
        #pragma omp section 
        // producer
        {
            for (int i = 0; i < SIZE; i++)
            {
                // produce a random number
                usleep(1e5 * (rand() % 3)); // time to produce
                int r = rand() % 100;
                
                #pragma omp critical
                {
                    // put to the buffer
                    buffer[count] = r;
                    count++;
                }
                
                printf("%d produced %d\n", i, r);
            }
        }

        #pragma omp section
        // consumer
        {
            int consumed = 0;
            while (consumed < SIZE)
            {
                if (count > 0)
                {
                    int r;
                    #pragma omp critical
                    {
                        // take from the buffer (consume)
                        r = buffer[count - 1];
                        count--;
                    }
                    usleep(1e5 * (rand() % 2)); // time to consume
                    
                    consumed++;
                    printf("consumed %d\n", r);
                }
            }
        }
    }

    return 0;
}