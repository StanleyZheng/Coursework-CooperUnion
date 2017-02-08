#include <errno.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/wait.h>
#include <unistd.h>

#define N_PROC 64       //number of "processors"/children in terms of implementation
#define NUMTRY 2000000  //number of times each child will perform an increment

int *pint;
char *spinlock;
int tas(volatile char *lock);


//test the case for incrementing an int without TAS
int without_tas(void){
    int i, j;
    
    for(i = 0; i < N_PROC; ++i){
        switch(fork()){
            case -1:
                fprintf(stderr, "Could not fork on the %d time(s)\n", i + 1);
                exit(-1);
                
            case 0:
                for(j = 0; j < NUMTRY; ++j)
                    ++(*pint);
                exit(0);
        }
    }
    
    while(wait(NULL)){
        if(errno == ECHILD)
            break;
    }
    
    fprintf(stdout, "Without lock:\n\tadditions tried: %d\n\tadditions counted: %d\n", N_PROC * NUMTRY, *pint);
    return 0;
}


//test the case for incrementing an int with TAS
int with_tas(void){
    int i, j;
    
    for(i = 0; i < N_PROC; ++i){
        switch(fork()){
            case -1:
                fprintf(stderr, "Could not fork on the %d time(s)\n", i + 1);
                exit(-1);
                
            case 0:
                for(j = 0; j < NUMTRY; ++j){
                    while(tas(spinlock) != 0)
                        ;
                    ++(*pint);
                    *spinlock = 0;
                }
                exit(0);
        }
    }
    
    while(wait(NULL)){
        if(errno == ECHILD)
            break;
    }
    
    fprintf(stdout, "With lock:\n\tadditions tried: %d\n\tadditions counted: %d\n", N_PROC * NUMTRY, *pint);
    return 0;
}


int main(void){
    if((pint = (int *)mmap(NULL, 100, PROT_READ|PROT_WRITE, MAP_SHARED|MAP_ANONYMOUS, -1, 0)) == MAP_FAILED){
        fprintf(stderr, "Error: %s - Could not mmap the shared int 'pint'\n", strerror(errno));
        exit(-1);
    }
    spinlock = (char *)(pint + 1);

    *pint = 0;
    without_tas();

    *pint = 0;
    errno = 0;
    with_tas();

    return 0;
}