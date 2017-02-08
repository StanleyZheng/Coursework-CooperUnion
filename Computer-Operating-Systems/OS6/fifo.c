#include "fifo.h"
#include <stdio.h>


void fifo_init(struct fifo *f){
    sem_init(&f->empty, FIFO_BUFSIZ);
    sem_init(&f->filled, 0);
    sem_init(&f->rd_lock, 1);
    sem_init(&f->wr_lock, 1);
    f->front = 0;
    f->back = 0;

    return;
}


void fifo_wr(struct fifo *f, unsigned long d){
    while(1){
        sem_wait(&f->wr_lock);
        if(sem_try(&f->empty) == 1)
            break;
        sem_inc(&f->wr_lock);
    }

    f->fifo_queue[f->back] = d;
    //f->back = (f->back + 1) % FIFO_BUFSIZ;
    if(++f->back == FIFO_BUFSIZ)
        f->back = 0;
    sem_inc(&f->filled);
    sem_inc(&f->wr_lock);

    return;
}


unsigned long fifo_rd(struct fifo *f){
    unsigned long d;

    while(1){
        sem_wait(&f->rd_lock);
        if(sem_try(&f->filled) == 1)
            break;
        sem_inc(&f->rd_lock);
    }

    d = f->fifo_queue[f->front];
    //f->front = (f->front + 1) % FIFO_BUFSIZ;
    if(++f->front == FIFO_BUFSIZ)
        f->front = 0;
    sem_inc(&f->empty);
    sem_inc(&f->rd_lock);

    return d;
}