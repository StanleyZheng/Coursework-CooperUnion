/*
    ECE357: Computer Operating Systems PS8
    Context Switch / Scheduler
    Stanley Zheng
    test.c
*/

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/types.h>
#include <unistd.h>
#include "sched.h"


void parent_fcn(){
    int i, childpid, retval;
    for(i = -2; i < 3; ++i){
        if((childpid = sched_wait(&retval)) != -1)
            fprintf(stderr, "Parent pid: %d received retval of %d from child pid: %d\n", sched_getpid(), retval, childpid);
        else
            fprintf(stderr, "Parent pid: %d has no children to wait for\n", sched_getpid());
    }
    fprintf(stderr, "Total ticks: %d\n", sched_gettick());
    exit(0);
}


void child_fcn(){
    long long i;
    for(i = 0; i < 2000000001; ++i){
        if(i % 200000000 == 0){
            fprintf(stderr, "In PID %d, i = %lld\n", sched_getpid(), i);
            kill(getpid(), SIGABRT);
        }
    }
    sched_exit(0);
}


void init_fcn(){
    int i;
    for(i = -2; i < 2; ++i){
        switch(sched_fork()){
            case -1:
                fprintf(stderr, "Error in sched_fork\n");
                break;
            case 0:
                sched_nice(i * 10);
                child_fcn();
        }
    }
    parent_fcn();
}


int main(void){
	sched_init(init_fcn);
	return 0;
}