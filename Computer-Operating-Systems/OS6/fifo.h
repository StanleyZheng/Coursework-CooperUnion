#ifndef _FIFO_H
#define _FIFO_H

#include "sem.h"
#define N_PROC 64
#define FIFO_BUFSIZ 4096

struct fifo {
    struct sem filled, empty;               //sems to keep track of how much of fifo is filled
    struct sem rd_lock, wr_lock;            //locks for num reader and writer
    unsigned long fifo_queue[FIFO_BUFSIZ];  //array to hold onto contents
    int front, back;                        //indexes of front and back of array
};

void fifo_init(struct fifo *f);
void fifo_wr(struct fifo *f, unsigned long d);
unsigned long fifo_rd(struct fifo *f);

#endif //_FIFO_H