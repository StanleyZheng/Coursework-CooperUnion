#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <sys/types.h>
#include <unistd.h>
#include "sem.h"

extern int my_procnum;
extern int my_pid;

int tas(volatile char *lock);

void handler(int sig){
	return;
}

void sem_init(struct sem *s, int count){
	s->count = count;
	s->lock = 0;
	if(signal(SIGUSR1, handler) == SIG_ERR){
        fprintf(stderr, "Error: %s - Could not catch SIGUSR1\n", strerror(errno));
        exit(-1);
    }
}

int sem_try(struct sem *s){
	int retval = 0;

	while(tas(&s->lock) != 0)
		;
	
	if(s->count){
		--s->count;
		retval = 1;
	}

	s->lock = 0;
	return retval;
}

void sem_wait(struct sem *s){
	sigset_t set;
retry:
	while(tas(&s->lock) != 0)
		;
	
	if(s->count > 0){
		--s->count;
		s->lock = 0;
	}
	else{
		s->lock = 0;
		s->sleeping[my_procnum] = my_pid;
		sigfillset(&set);
		sigdelset(&set, SIGUSR1);
		sigsuspend(&set);
		goto retry;
	}

	return;
}

void sem_inc(struct sem *s){
	int i;
	while(tas(&s->lock) != 0)
		;
	
	++s->count;
	for(i = 0; i < N_PROC; ++i){
		if(s->sleeping[i] != 0){
			kill(s->sleeping[i], SIGUSR1);
			s->sleeping[my_procnum] = 0;
		}
	}
	s->lock = 0;
	return;
}