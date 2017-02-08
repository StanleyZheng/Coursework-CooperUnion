#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>

#define CHILDNUM 10;  //number of forks
#define TRYNUM 1000;  //number of tries in sending signal per child


int sig_count;      //counter for the number of signals processed
void sig_handler(int signum){
    ++sig_count;
    return;
}


int sendsig(int signum){
    int i, j;
    pid_t pid = getpid();
    sig_count = 0;

    if(signal(signum, sig_handler) == SIG_ERR){
        fprintf(stderr, "Could not catch signal %d: %s\n", signum, strerror(errno));
        exit(-1);
    }

    for(i = 1; i <= CHILDNUM; i++){
        switch( fork() ){
            case -1:
                fprintf(stderr, "Could not fork on the %dth instance to send signal %d: %s\n", i, signum, strerror(errno));
                exit(-1);

            case 0:
                for(j = 1; j <= TRYNUM; j++){
                    if (kill(pid, signum) == -1){
                        fprintf(stderr, "Signal %d - Failed to send %d time(s) on %dth child: %s\n", signum, j, i, strerror(errno));
                        exit(-1);
                    }
                }
                exit(0);
        }
    }

    while( wait(NULL) ){
        if(errno == ECHILD)
            break;
    }

    fprintf(stdout, "Signal %d - Sent: %d - Recieved: %d\n", signum, CHILDNUM * TRYNUM, sig_count);
    return 0;
}


int main(void){
    sendsig(20);
    sendsig(30);
    sendsig(38);
    return 0;
}

