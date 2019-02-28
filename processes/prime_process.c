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

long* processFunc(long nextCand, long limit, long pCount) {
    static long returnData[3];
    while (nextCand < limit) {
        if (isPrime(nextCand)) {
            pCount++;
        }
        nextCand += 2;
    }
    returnData[0] = nextCand;
    returnData[1] = limit;
    returnData[2] = pCount;
    return returnData;
}

long calculatePerProcessLimit(long numProcesses) {
    return N / (numProcesses * 2);
}
    
int main(void) {
    // TODO: refactor to read from stdin
    int processLimit;
    printf("How many child processes would you like to spin up?\n");
    scanf("%i", &processLimit);
    printf("We will now fork the main process %i times.\n", processLimit);
    printf("Each time we work, we will give work to both the parent process and child process.\n");

    // 
    long pCount = 1;
    long nextCand = 3;
    long perProcessLimit = calculatePerProcessLimit(processLimit);
    long limit = 0;

    int cand_fd[2];
    int limit_fd[2];
    int prime_fd[2];

    pipe(cand_fd);
    pipe(limit_fd);
    pipe(prime_fd);

    struct timespec start, end;
    clock_gettime(CLOCK_REALTIME, &start);
    // separate into 2 loops first creates processes second waits
    for (int i = 0; i < processLimit; i++) {
        int pid = fork();
        int status;

        if (pid == 0) {
            // child process
            limit += perProcessLimit;
            printf("\x1b[31mchild process %i: \x1b[0m", i + 1);
            printf("nextCand: %li, limit: %li, pCount: %li\n", nextCand, limit, pCount);
            long* data = processFunc(nextCand, limit, pCount);
            write(cand_fd[1], &data[0], sizeof(&data[0]));
            write(limit_fd[1], &data[1], sizeof(&data[1]));
            write(prime_fd[1], &data[2], sizeof(&data[2]));
            exit(0);
        } else if (pid > 0) {
            // parent process
            // Wait on child
            waitpid(pid, &status, WUNTRACED);
            read(cand_fd[0], &nextCand, sizeof(nextCand));
            read(limit_fd[0], &limit, sizeof(limit));
            read(prime_fd[0], &pCount, sizeof(pCount));
            // handles rounding errors. Always set limit to N on last iteration
            if (i == (processLimit - 1)) {
                limit = N;
            } else {
                limit += perProcessLimit;
            }
            
            printf("\x1b[34mparent:\x1b[0m nextCand: %li, limit: %li, pCount: %li\n", nextCand,limit, pCount);

            long* data = processFunc(nextCand, limit, pCount);
            nextCand = data[0];
            limit = data[1];
            pCount = data[2];
        }
    }

    clock_gettime(CLOCK_REALTIME, &end);
    double time_spent = (end.tv_sec - start.tv_sec) +
    (end.tv_nsec - start.tv_nsec) / 1000000000.0f;
    
    printf("Time tasken: %f seconds\n", time_spent);
    printf("%ld primes found.\n", pCount);
}
