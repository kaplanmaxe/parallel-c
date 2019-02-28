#include <math.h>
#include <stdio.h>
#include <time.h>

typedef int bool;
#define FALSE 0
#define TRUE 1

#define N 10000000
//returns whether num is prime
bool isPrime(long num) {
    long limit = sqrt(num);
    for(long i=2; i<=limit; i++) {
        if(num % i == 0) {
            return FALSE;
        }
    }
    return TRUE;
}
    
int main(void) {
    int pCount = 1;
    long nextCand = 3;
    
    
    struct timespec start, end;
    clock_gettime(CLOCK_REALTIME, &start);
    
    while(nextCand < N) {
        if(isPrime(nextCand)) {
            pCount++;
        }
        nextCand += 2;
    }
    
    clock_gettime(CLOCK_REALTIME, &end);
    double time_spent = (end.tv_sec - start.tv_sec) +
    (end.tv_nsec - start.tv_nsec) / 1000000000.0f;
    
    printf("Time tasken: %f seconds\n", time_spent);
    printf("%d primes found.\n", pCount);
}