/*
    ECE357: Computer Operating Systems PS8
    Context Switch / Scheduler
    Stanley Zheng
    sched.h
*/

#ifndef SCHED_H
#define SCHED_H

#include "savectx.h"
#define SCHED_NUMPROC 256
#define SCHED_READY 1
#define SCHED_RUNNING 2
#define SCHED_SLEEPING 3
#define SCHED_ZOMBIE 4


struct sched_proc {
    int cputime;
    int state;
    int dynamic_priority;
    int pid;
    int ppid;
    int isin_inqueue;
    int retval;
    struct savectx context;
    void *stack;
    int static_priority;
};


struct sched_waitq {
    struct sched_proc *list[SCHED_NUMPROC];
    int count;
    int beginning;
};


struct sched_all {
    struct sched_proc list[SCHED_NUMPROC];
    int count;
};


void sched_init(void (*init_fcn)());
int sched_fork();
void sched_exit(int code);
int sched_wait(int *exit_code);
void sched_nice(int niceval);
int sched_getpid();
int sched_getppid();
int sched_gettick();
void sched_ps();
void sched_switch();
void sched_tick();


#endif //SCHED_H