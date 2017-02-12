#ifndef _SEM_H
#define _SEM_H

#define N_PROC 64

struct sem {
	int count;
    int sleeping[N_PROC];
	char lock;
};

void sem_init(struct sem *s, int count);
int sem_try(struct sem *s);
void sem_wait(struct sem *s);
void sem_inc(struct sem *s);

#endif //_SEM_H