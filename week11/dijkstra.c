// Source:
// https://gitlab.unimelb.edu.au/souravg/spartan-examples/-/blob/master/OpenMP/Dijkstra.c

// Dijkstra shortest-path finder in a bidirectional graph;
// finds the shortest path from vertex 0 to all others

// usage:  dijkstra nv nth print
// where nv is the size of the graph
//       nth is the number of threads
//       print is 1 if graph and min distances are to be printed out, 0
//       otherwise

#include <omp.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

// global variables, shared by all threads by default
int nv,            // number of vertices
    *checked,      // shows if vertices were checked
    nth,           // number of threads
    chunk,         // number of vertices handled by each thread
    md,            // current minimal distance over all threads
    mv,            // vertex which achieves that minimal distance
    largeint = -1; // max possible unsigned int

unsigned *d,   // distances between vertices; "d[i][j]" is d[i * nv + j]
    *min_dist; // min distances found so far

void init(int ac, char **av) {
    int i, j, tmp;

    nv = atoi(av[1]);
    nth = atoi(av[2]);

    if (nv % nth != 0) {
        printf("number of vertices must be divisible by number of threads\n");
        exit(1);
    }

    chunk = nv / nth;
    omp_set_num_threads(nth);
    srand(time(NULL));

    d = malloc(nv * nv * sizeof(int));
    min_dist = malloc(nv * sizeof(int));
    checked = malloc(nv * sizeof(int));

    // random graph
    for (i = 0; i < nv; i++) {
        for (j = i; j < nv; j++) {
            if (j == i) {
                d[i * nv + i] = 0;
            } else {
                d[nv * i + j] = 1 + rand() % 9;
                d[nv * j + i] = 1 + rand() % 9;
                // d[nv * j + i] = d[nv * i + j]; // symmetrical graph
            }
        }
    }

    // initialize variables
    for (i = 1; i < nv; i++) {
        checked[i] = 0;
        min_dist[i] = d[i];
    }
}

// finds closest to 0 among not checked, among s through e
void find_th_min(int s, int e, unsigned *d, int *v) {
    int i;
    *d = largeint;
    for (i = s; i <= e; i++) {
        usleep(1);
        if (!checked[i] && min_dist[i] < *d) {
            *d = d[i];
            *v = i;
        }
    }
}

// for each i in [s, e], ask whether a shorter path to i exists, through mv
void update_distance(int s, int e) {
    int i;
    for (i = s; i <= e; i++) {
        usleep(1);
        if (min_dist[mv] + d[mv * nv + i] < min_dist[i]) {
            min_dist[i] = min_dist[mv] + d[mv * nv + i];
        }
    }
}

void dijkstra() {
#pragma omp parallel
    {
        int start, end, // start and end vertices for the thread
            i,          // index to iterate through nv vertices
            th_mv, // vertex which attains the min value in the thread's chunk
            th = omp_get_thread_num();
        unsigned th_md; // min value found by this thread

        start = th * chunk;
        end = start + chunk - 1;

        for (i = 0; i < nv; i++) {
// find closest vertex to 0 among not checked;
// each thread finds the closest in its group
// then overall closest is determined
#pragma omp single
            {
                md = largeint;
                mv = 0;
            }

            find_th_min(start, end, &th_md, &th_mv);

// update overall minimal distance if current th_md is smaller
#pragma omp critical
            {
                if (th_md < md) {
                    md = th_md;
                    mv = th_mv;
                }
            }
#pragma omp barrier

// mark new vertex as checked
#pragma omp single
            { checked[mv] = 1; }

            // update the thread's section of minimal distance
            update_distance(start, end);
#pragma omp barrier
        }
    }
}

int main(int argc, char **argv) {
    int i, j, print;

    // initialize the problem
    init(argc, argv);

    // parallel calculations
    dijkstra();

    // back to single thread
    print = atoi(argv[3]);
    if (print) {
        printf("graph weights:\n");
        for (i = 0; i < nv; i++) {
            for (j = 0; j < nv; j++) {
                printf("%u\t", d[nv * i + j]);
            }
            printf("\n");
        }
        printf("minimum distances:\n");
        for (i = 1; i < nv; i++) {
            printf("from 0 to %d: %u\n", i, min_dist[i]);
        }
    }

    exit(0);
}