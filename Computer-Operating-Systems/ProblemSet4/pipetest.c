#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

int main(void){
    int fds[2];
    int wrote;
    int counter = 0;
    char testchar[1] = "c";


    if(pipe(fds) < 0){
        fprintf(stderr, "Failed to create a pipe: %s\n", strerror(errno));
        return -1;
    }

    if(fcntl(fds[1], F_SETFL, O_NONBLOCK) < 0){
        fprintf(stderr, "Could not set NONBLOCK flag: %s\n", strerror(errno));
        return -1;
    }

    while(wrote = write(fds[1], testchar, sizeof(testchar)) > 0){
        counter += wrote;
    }

    if (errno == EAGAIN)
        fprintf(stdout, "Capacity of pipe(in bytes): %d\n", counter);
    else
        fprintf(stderr, "Error while writing to input of pipe: %s\n", strerror(errno));


    return 0;
}
