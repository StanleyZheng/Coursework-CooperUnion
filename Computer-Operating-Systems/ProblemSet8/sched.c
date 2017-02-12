/*
    ECE357: Computer Operating Systems PS8
    Context Switch / Scheduler
    Stanley Zheng
    sched.c
*/

#include <errno.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/time.h>
#include <sys/wait.h>
#include "sched.h"
#include "adjstack.c"

#define STACK_SIZE 65536
#define TICK_RATE 100000 //microseconds

int ticks;
sigset_t fullset;
struct sched_proc *current;
struct sched_all allprocs;
struct sched_waitq in_queue, out_queue; // for the simplified O(1) scheduling


void sched_init(void (*init_fcn)()){
    struct itimerval ticker;
    ticker.it_interval.tv_sec = 0;
    ticker.it_interval.tv_usec = TICK_RATE;
    ticker.it_value.tv_sec = 0;
    ticker.it_value.tv_usec = TICK_RATE;

    sigfillset(&fullset);

    if(signal(SIGVTALRM, sched_tick) == SIG_ERR){
        fprintf(stderr, "Error - setting SIGVTALRM handler: %s\n", strerror(errno));
        exit(-1);
    }
    if(signal(SIGABRT, sched_ps) == SIG_ERR){
        fprintf(stderr, "Error - setting SIGABRT handler: %s\n", strerror(errno));
        exit(-1);
    }
    if(setitimer(ITIMER_VIRTUAL, &ticker, NULL) == -1){
        fprintf(stderr, "Error - running setitimer: %s\n", strerror(errno));
        exit(-1);
    }

    // Initialize first process
    struct sched_proc firstproc;
    void *newsp;
    if((newsp = mmap(0, STACK_SIZE, PROT_READ|PROT_WRITE, MAP_PRIVATE|MAP_ANONYMOUS, 0, 0)) == MAP_FAILED){
        fprintf(stderr, "Error - Failed to mmap 'newsp' for first process: %s\n", strerror(errno));
        exit(-1);
    }

    firstproc.cputime = 0;
    firstproc.state = SCHED_RUNNING;
    firstproc.dynamic_priority = 20;
    firstproc.pid = 1;
    firstproc.ppid = 0;
    firstproc.stack = newsp;
    firstproc.static_priority = 20;
    firstproc.retval = 0;

    allprocs.list[allprocs.count++] = firstproc;
    current = &allprocs.list[0];

    struct savectx firstcontext;
    firstcontext.regs[JB_BP] = newsp + STACK_SIZE;
    firstcontext.regs[JB_SP] = newsp + STACK_SIZE;
    firstcontext.regs[JB_PC] = init_fcn;
    restorectx(&firstcontext, 0);
}


int sched_fork(){
    int i, retval;
    struct sched_proc childproc;
    void *newsp;
    sigprocmask(SIG_BLOCK, &fullset, NULL);

    if(allprocs.count == SCHED_NUMPROC){
        fprintf(stderr, "Error - maximum number of procs reached when forking\n");
        sigprocmask(SIG_UNBLOCK, &fullset, NULL);
        return -1;
    }

    if((newsp = mmap(0, STACK_SIZE, PROT_READ|PROT_WRITE, MAP_PRIVATE|MAP_ANONYMOUS, 0, 0)) == MAP_FAILED){
        fprintf(stderr, "Error - failed to mmap 'newsp' for child: %s\n", strerror(errno));
        sigprocmask(SIG_UNBLOCK, &fullset, NULL);
        return -1;
    }

    memcpy(newsp, current->stack, STACK_SIZE);
    childproc.cputime = 0;
    childproc.state = SCHED_READY;
    childproc.dynamic_priority = 20;
    childproc.ppid = current->pid;
    childproc.stack = newsp;
    childproc.static_priority = 20;
    childproc.retval = 0;

    adjstack(newsp, newsp + STACK_SIZE, childproc.stack - current->stack);
    for(i = 0; allprocs.list[i].pid != 0; ++i);
    childproc.pid = i + 1;

    allprocs.list[i] = childproc;
    ++allprocs.count;
    if(in_queue.beginning + in_queue.count < SCHED_NUMPROC)
        in_queue.list[in_queue.beginning + in_queue.count++] = &allprocs.list[i];
    else
        in_queue.list[in_queue.beginning + in_queue.count++ - SCHED_NUMPROC] = &allprocs.list[i];


    if(savectx(&allprocs.list[i].context) == 0){
        allprocs.list[i].context.regs[JB_BP] += allprocs.list[i].stack - current->stack;
        allprocs.list[i].context.regs[JB_SP] += allprocs.list[i].stack - current->stack;
        retval = allprocs.list[i].pid;
    }
    else
        retval = 0;

    sigprocmask(SIG_UNBLOCK, &fullset, NULL);
    return retval;
}


void sched_exit(int code){
    int i;
    sigprocmask(SIG_BLOCK, &fullset, NULL);

    current->state = SCHED_ZOMBIE;
    current->retval = code;

    for(i = 0; i < SCHED_NUMPROC; ++i){
        if(allprocs.list[i].pid != 0 && allprocs.list[i].pid == current->ppid && allprocs.list[i].state == SCHED_SLEEPING){
            allprocs.list[i].state = SCHED_READY;
            if(in_queue.beginning + in_queue.count < SCHED_NUMPROC)
                in_queue.list[in_queue.beginning + in_queue.count++] = &allprocs.list[i];
            else
                in_queue.list[in_queue.beginning + in_queue.count++ - SCHED_NUMPROC] = &allprocs.list[i];
            allprocs.list[i].isin_inqueue = 1;
            break;
        }
    }

    sigprocmask(SIG_UNBLOCK, &fullset, NULL);
    sched_switch();
}


int sched_wait(int *exit_code){
    int i, count, childpid, retval = 0;

    while(!retval){
        count = 0;
        childpid = 0;
        sigprocmask(SIG_BLOCK, &fullset, NULL);

        for(i = 0; i < SCHED_NUMPROC; ++i){
            if(count == allprocs.count){
                retval = -1;
                break;
            }
            if(allprocs.list[i].pid == 0)
                continue;

            ++count;
            if(allprocs.list[i].ppid == current->pid){
                if(allprocs.list[i].state == SCHED_ZOMBIE){
                    childpid = allprocs.list[i].pid;
                    *exit_code = allprocs.list[i].retval;
                    munmap(allprocs.list[i].stack, STACK_SIZE);
                    allprocs.list[i].pid = 0;
                    --allprocs.count;
                    retval = childpid;
                    break;
                }
                else{
                    current->state = SCHED_SLEEPING;
                    sched_switch();
                    retval = 0;
                    break;
                }
            }
        }
    }

    sigprocmask(SIG_UNBLOCK, &fullset, NULL);
    return retval;
}


void sched_nice(int niceval){
    if(niceval > 19)
        niceval = 19;
    else if(niceval < -20)
        niceval = -20;
    
    current->static_priority = niceval + 20;
    return;
}


int sched_getpid(){
    return current->pid;
}


int sched_getppid(){
    return current->ppid;
}


int sched_gettick(){
    return ticks;
}


void sched_ps(){
    int i;
    int count = 0;
    int dpriority;
    
    sigprocmask(SIG_BLOCK, &fullset, NULL);
    fprintf(stderr, "PID\tPPID\tSTATE\tSTACK ADDR\tSTATIC\tDYNAMIC\tCPU\n");
    for(i = 0; i < SCHED_NUMPROC; ++i){
        if(count == allprocs.count)
            break;
        if(allprocs.list[i].pid == 0)
            continue;
        ++count;

        if(allprocs.list[i].isin_inqueue == 1 || allprocs.list[i].state == SCHED_RUNNING)
            dpriority = allprocs.list[i].dynamic_priority;
        else
            dpriority = 40;

        fprintf(stderr, "%d\t%d\t%d\t%p\t%d\t%d\t%d\n", allprocs.list[i].pid, allprocs.list[i].ppid, allprocs.list[i].state,
            allprocs.list[i].stack, allprocs.list[i].static_priority, dpriority, allprocs.list[i].cputime);
    }

    sigprocmask(SIG_UNBLOCK, &fullset, NULL);
    return;
}


void sched_switch(){
    sigprocmask(SIG_BLOCK, &fullset, NULL);

    if(current->state == SCHED_READY)
        out_queue.list[out_queue.count++] = current;

    if(in_queue.count == 0){
        int i, count = 0;

        in_queue = out_queue;
        for(i = 0; i < SCHED_NUMPROC; ++i){
            if(count == allprocs.count)
                break;
            ++count;

            if(allprocs.list[i].pid != 0 && allprocs.list[i].state == SCHED_READY)
                allprocs.list[i].isin_inqueue = 1;
        }
        out_queue.count = 0;
    }
    
    if(savectx(&current->context) == 0){
        current = in_queue.list[in_queue.beginning];
        if(++in_queue.beginning == SCHED_NUMPROC)
            in_queue.beginning = 0;
        --in_queue.count;
        current->state = SCHED_RUNNING;
        current->isin_inqueue = 0;
        restorectx(&current->context, 1);
    }

    sigprocmask(SIG_UNBLOCK, &fullset, NULL);
    return;
}


void sched_tick(){
    sigprocmask(SIG_BLOCK, &fullset, NULL);
    ++ticks;
    ++current->cputime;

    if(++current->dynamic_priority > 39 && current->state != SCHED_ZOMBIE){
        current->dynamic_priority = current->static_priority;
        current->state = SCHED_READY;
        sched_switch();
    }
    
    sigprocmask(SIG_UNBLOCK, &fullset, NULL);
    return;
}
