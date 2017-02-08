//tcp_recv.c

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
    int sockfd, port, bits_read, bits_wrote, difference, s;
    unsigned int len;
    int buf_size = 4096; 
    long diff, totalbits = 0;
    double rate;
    char text[buf_size];
    struct timeval start, end;
    struct sockaddr_in sin;
    struct hostent *host;
    
    if (argc != 2){
        fprintf(stderr, "Error - usage: tcp_recv [port]\n");
        return -1; 
    }
    if ((port = atoi(argv[1])) < 1){
        fprintf(stderr, "Error - Use of invalid port %s\n", argv[2]);
        return -1; 
    }
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0){
        fprintf(stderr, "Error - Could not obtain socket: %s\n", strerror(errno));
        return -1; 
    }

    sin.sin_family = AF_INET; 
    sin.sin_port = htons(port); 
    sin.sin_addr.s_addr = INADDR_ANY;

    if (bind(sockfd, (struct sockaddr *)&sin, sizeof sin) < 0){
        fprintf(stderr, "Error - Could not bind: %s\n", strerror(errno));
        close(sockfd);
        return -1;
    }

    if (listen(sockfd, 1) < 0){
        fprintf(stderr, "Error - Could listen: %s\n", strerror(errno));
        return -1; 
    }

    len = sizeof sin;
    if ((s = accept(sockfd, (struct sockaddr *)&sin, &len)) < 0){
        fprintf(stderr, "Error - Could not accept: %s\n", strerror(errno));
        return -1; 
    }
    
    if ((host = gethostbyaddr(&sin.sin_addr, sizeof sin.sin_addr, AF_INET )) == NULL)
        fprintf(stderr, "Error - Hostname could not be resolved\n"); 
    else
        fprintf(stderr, "Connection from %s\n", host->h_name);
    
    gettimeofday(&start, NULL); 
    while (1){
        if ((bits_read = read(s, text, buf_size)) == 0) 
            break;
        if (bits_read < 0){
            fprintf(stderr, "Error - Cannot read: %s\n", strerror(errno));
            return -1; 
        }
        totalbits += bits_read;
        difference = 0;
        
        while(1){
            if ((bits_wrote = write(1, text + difference, bits_read)) == bits_read)
                break;
            if (bits_wrote < 0){
                fprintf(stderr, "Error - Cannot write to socket: %s\n", strerror(errno));
                return -1; 
            }
            else if (bits_wrote < bits_read){ //in case of partial write 
                difference += bits_wrote;
                bits_read -= bits_wrote;
            }
        }
    }
    gettimeofday(&end, NULL);

    if (close(sockfd) < 0){
        fprintf(stderr, "Error - Could not close socket: %s\n", strerror(errno));
        return -1;
    }
    
    diff = end.tv_sec * 1000000L + end.tv_usec - start.tv_sec * 1000000L - start.tv_usec;
    rate = ((double)totalbits / 1000000) / ((double)diff / 1000000); 
    fprintf(stderr, "%ld bytes received at %.2fMB/s\n", totalbits, rate);
    return 0; 
}