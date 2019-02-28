#include <math.h>
#include <stdio.h>
#include <time.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <sys/types.h> 

typedef int bool;
#define FALSE 0
#define TRUE 1

#define N 10000000  // 10 Million

int* pids;
int pid_fd[2];
int prime_fd[2];

// long pCount = 1;
long limit = 0;

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

int processFunc(long first, long last) {
    long nextCand;
    // If first is even, primeCount will always be 0. We want
    // to start off with an odd number
    if (first % 2 == 0) {
        nextCand = first - 1;
    } else {
        nextCand = first;
    }
    int primeCount = 0;
    
    while (nextCand < last) {
        if (isPrime(nextCand)) {
            primeCount++;
        }
        nextCand += 2;
    }
    return primeCount;
}

long calculatePerProcessLimit(long numProcesses) {
    return N / numProcesses;
}

long spawn(int num_processes) {
    long pCount = 0;
    pids = malloc(num_processes * sizeof(int));

    long perProcessLimit = calculatePerProcessLimit(num_processes);

    pipe(prime_fd);
    pipe(pid_fd);

    
    // We spin off n processes and give them work
    for (int i = 0; i < num_processes; i++) {
        int pid = fork();
        if (pid != 0) {
            // Parent process
            // Write thread id to pipe
            write(pid_fd[1], &pid, sizeof(pid));
        } else {
            // Child process
            limit += perProcessLimit;
            long first;
            long last;
            if (i == 0) {
                first = 3;
                last = perProcessLimit;
            } else if (i == (num_processes - 1)) {
                first = (perProcessLimit * i) + 1;
                last = N;
            } else {
                first = (perProcessLimit * i) + 1;
                last = perProcessLimit * (i + 1);
            }
            int pCount = processFunc(first, last);
            printf("\x1b[34mprocess %i:\x1b[0m found %i prime numbers between %li and %li\n", i, pCount, first, last);
            write(prime_fd[1], &pCount, sizeof(pCount));
            exit(0);
        }
    }

    // We get the all the child process ids and aggregate results
    for (int i = 0; i < num_processes; i++) {
        int pid;
        read(pid_fd[0], &pid, sizeof(pid));
        pids[i] = pid;
        int status;
        int primeCount;
        waitpid(pids[i], &status, WUNTRACED);
        read(prime_fd[0], &primeCount, sizeof(primeCount));
        pCount += primeCount;
    }
    return pCount;
}
    
int main(void) {
    int num_processes;
    printf("How many processes would you like to spawn?\n");
    scanf("%i", &num_processes);

    struct timespec start, end;
    clock_gettime(CLOCK_REALTIME, &start);

    long pCount = 1;
    if (num_processes > 0) {
        pCount += spawn(num_processes);
    } else {
        pCount += processFunc(3, N);
        printf("\x1b[34mprocess 0:\x1b[0m found %li prime numbers between 3 and %i\n", pCount, N);
    }

    clock_gettime(CLOCK_REALTIME, &end);
    double time_spent = (end.tv_sec - start.tv_sec) +
    (end.tv_nsec - start.tv_nsec) / 1000000000.0f;
    
    printf("Time tasken: %f seconds\n", time_spent);
    printf("%ld primes found.\n", pCount);
}
