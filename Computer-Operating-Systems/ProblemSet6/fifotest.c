#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/mman.h>
#include <sys/wait.h>
#include <unistd.h>
#include "fifo.h"

#define N_PROC 64       //number of "processors"/children in terms of implementation
#define NUMTRY 1000     //number of times each child will perform an increment

int my_procnum;
int my_pid;
struct fifo *pfifo;

int main(void){
    if((pfifo = (struct fifo *)mmap(NULL, sizeof(struct fifo), PROT_READ|PROT_WRITE, MAP_SHARED|MAP_ANONYMOUS, -1, 0)) == MAP_FAILED){
        fprintf(stderr, "Error: %s - Could not mmap for shared fifo\n", strerror(errno));
        exit(-1);
    }    
    fifo_init(pfifo);

    int i, j;
    for(i = 0; i < N_PROC; i++){
        switch(fork()){
            case -1:
                fprintf(stderr, "Could not fork the %d writer\n", i + 1);
                exit(-1);
                
            case 0:
                my_procnum = i;
                my_pid = getpid();
                for(j = 0; j < NUMTRY; j++)
                    fifo_wr(pfifo, ((unsigned long)i << 32) | j);
                exit(0);
        }
    }

    switch(fork()){
        case -1:
            fprintf(stderr, "Could not fork for reader\n");
            exit(-1);
            
        case 0:
            my_pid = getpid();
            unsigned long pop;
            for(j = 0; j < NUMTRY * N_PROC; j++){
                pop = fifo_rd(pfifo);
                fprintf(stdout, "processor %d\tread: %d\n", (int)(pop>>32), (int)((pop<<32)>>32));
            }
            exit(0);
    }

    while(wait(NULL)){
        if(errno == ECHILD)
            break;
    }
    return 0;
}