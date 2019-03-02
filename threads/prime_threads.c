#include <math.h>
#include <stdio.h>
#include <time.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <sys/types.h> 

typedef int bool;
#define FALSE 0
#define TRUE 1

#define N 10000000  // 10 Million

long pCount = 1;
pthread_mutex_t lock;

// returns whether num is prime
bool isPrime(long num) {
    long limit = sqrt(num);
    for(long i=2; i<=limit; i++) {
        if(num % i == 0) {
            return FALSE;
        }
    }
    return TRUE;
}

struct PrimeParams {
    long first, last;
    int thread_id;
};

void* threadFunc(void* params) {
    struct PrimeParams *p = (struct PrimeParams*)params;
    long nextCand;
    // If first is even, primeCount will always be 0. We want
    // to start off with an odd number
    if ((*p).first % 2 == 0) {
        nextCand = (*p).first - 1;
    } else {
        nextCand = (*p).first;
    }
    // We create a local instance here so we don't have to block other threads
    // N times. We can just block when we update pCount which is O(1)
    int primeCount = 0;
    while (nextCand < (*p).last) {
        if (isPrime(nextCand)) {
            primeCount++;
        }
        nextCand += 2;
    }
    // Critical section to update global pCount
    pthread_mutex_lock(&lock);
    pCount += primeCount;
    pthread_mutex_unlock(&lock);

    printf("\x1b[34mthread %i:\x1b[0m found %li prime numbers between %li and %li\n", (*p).thread_id, pCount, (*p).first, (*p).last);
    // Free memory and exit
    free(params);
    pthread_exit(NULL);
}

void spawn(int num_threads) {
    pthread_t* threads = malloc(num_threads * sizeof(pthread_t));
    struct PrimeParams *params;
    long perThread = N / num_threads;
    for (int i = 0; i < num_threads; i++) {
        params = malloc(sizeof(struct PrimeParams));
        (*params).thread_id = i;
        if (i == 0) {
            (*params).first = 3;
            (*params).last = perThread;
        } else if (i == (num_threads - 1)) {
            (*params).first = (perThread * i) + 1;
            (*params).last = N;
        } else {
            (*params).first = (perThread * i) + 1;
            (*params).last = perThread * (i + 1);
        }
        pthread_create(&threads[i], NULL, threadFunc, (void *) params);
    }

    // Wait for threads to finish
    int rc;
    for (int i = 0; i < num_threads; i++) {
        int* result = NULL;
        rc = pthread_join(threads[i], NULL);
    }
}
    
int main(void) {
    int num_threads;
    printf("How many threads would you like to spin up?\n");
    scanf("%i", &num_threads);

    struct timespec start, end;
    clock_gettime(CLOCK_REALTIME, &start);
    // We'll always start at least one thread. Otherwise
    // We will run into errors given func signature.
    // Could probably be overloaded though out of scope.
    if (num_threads == 0) {
        num_threads = 1;
    }
    spawn(num_threads);

    clock_gettime(CLOCK_REALTIME, &end);
    double time_spent = (end.tv_sec - start.tv_sec) +
    (end.tv_nsec - start.tv_nsec) / 1000000000.0f;
    
    printf("Time tasken: %f seconds\n", time_spent);
    printf("%ld primes found.\n", pCount);
}