//tcp_send.c

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

int main(int argc, char **argv){
    int sockfd, port;
    int buf_size = 4096, b_read, b_write, difference; 
    long diff, total = 0;
    double rate;
    char text[buf_size];
    struct timeval start, end;
    struct sockaddr_in sin;
    struct hostent *host;
    struct linger lngr;
    lngr.l_onoff = 1;
    lngr.l_linger = 30;

    if (argc != 3){
        fprintf(stderr, "Error - usage: tcp_send [hostname] [port]\n");
        return -1; 
    }
    if ((host = gethostbyname(argv[1])) == NULL){
        fprintf(stderr, "Error - Hostname not found\n");
        return -1; 
    }
    if ((port = atoi(argv[2])) < 1){
        fprintf(stderr, "Error - Invalid port %s\n", argv[2]);
        return -1; 
    }
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0){
        fprintf(stderr, "Error - Could not obtain socket: %s\n", strerror(errno));
        return -1; 
    }
    if (setsockopt(sockfd, SOL_SOCKET, SO_LINGER, &lngr, sizeof lngr) < 0){
        fprintf(stderr, "Error - Could not linger: %s\n", strerror(errno));
        return -1;
    }

    sin.sin_family = AF_INET;
    sin.sin_port = htons(port);
    memcpy(&sin.sin_addr.s_addr, host->h_addr, host->h_length);
    
    if (connect(sockfd, (struct sockaddr *)&sin, sizeof sin) < 0){
        fprintf(stderr, "Error - Unable to connect: %s\n", strerror(errno));
        return -1; 
    }

    gettimeofday(&start, NULL);
    while (1){
        if ((b_read = read(0, text, buf_size)) == 0)
            break;
        if (b_read < 0){
            fprintf(stderr, "Error - Cannot read from stdin: %s\n", strerror(errno)); 
            return -1;
        }

        total += b_read;
        difference = 0;

        while(1){
            if ((b_write = write(sockfd, text + difference, b_read)) == b_read)
                break;
            if (b_write < 0){
                fprintf(stderr, "Error - Cannot write to socket: %s\n", strerror(errno));
                return -1; 
            }
            else if (b_write < b_read){ //in case of partial write
                difference += b_write;
                b_read -= b_write;
            }
        }
    }
    gettimeofday(&end, NULL);

    if (close(sockfd) < 0){
        fprintf(stderr, "Error - Could not close socket: %s\n", strerror(errno));
        return -1; 
    }

    diff = end.tv_sec * 1000000L + end.tv_usec - start.tv_sec * 1000000L - start.tv_usec;
    rate = ((double)total / 1000000) / ((double)diff / 1000000); 
    fprintf(stderr, "%ld bytes sent\n", total);
    fprintf(stderr, "%.2f MB/s\n", rate);
    return 0; 
}