//
//  client.c
//  serv
//
//  Created by Ben Oliver on 08/11/15.
//  Copyright © 2015 Ben Oliver. All rights reserved.
//

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <assert.h>
#include <sys/types.h>


#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h> // close
#include <netdb.h>
#include <sys/select.h> // select


#define BUFFER_SIZE      512
#define DEFAULT_PROTOCOL 0


typedef struct stream_s {
    char readbuff[BUFFER_SIZE];
    char writebuff[BUFFER_SIZE];
    int  sockfd;
} stream;


int main(int argc, const char * argv[]) {
    
    if (argc != 3) {
        printf("<host> <port>\n");
        exit(EXIT_FAILURE);
    }
    
    const char* SERVER_IP = argv[1];
    const char* SERVER_PORT = argv[2];
    
    struct addrinfo hints, *servinfo, *p;
    
    stream stream;
    stream.sockfd = -1;
    
    errno = 0;
    
    memset(&hints, 0, sizeof hints);
    hints.ai_family   = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    
    
    int info;
    if ((info = getaddrinfo(SERVER_IP, SERVER_PORT, &hints, &servinfo)) != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(info));
        exit(EXIT_FAILURE);
    }
    
    for (p = servinfo; p != NULL; p = p->ai_next) {
        
        errno = 0;
        
        if ((stream.sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1) {
            // perror("socket");
            continue;
        }
        if (connect(stream.sockfd, p->ai_addr, p->ai_addrlen) == -1) {
            close(stream.sockfd);
            stream.sockfd = -1;
            // perror("connect");
            continue;
        }
        
        break;
    }
    
    freeaddrinfo(servinfo); // all done with this structure
    
    if (stream.sockfd == -1) {
        perror("no connection possible");
        exit(EXIT_FAILURE);
    }
    
    
    fd_set readset;
    FD_ZERO(&readset);
    FD_SET(stream.sockfd, &readset);
    FD_SET(STDIN_FILENO, &readset);
    
    while (1) {
        
        if (select(stream.sockfd+1, &readset, NULL, NULL, NULL) > 0) {
        
            if (FD_ISSET(STDIN_FILENO, &readset)) {
                read(STDIN_FILENO, stream.writebuff, BUFFER_SIZE);
                send(stream.sockfd, stream.writebuff, strlen(stream.writebuff), 0);
            }
            
            if (FD_ISSET(stream.sockfd, &readset)) {
                if (recv(stream.sockfd, stream.readbuff, BUFFER_SIZE, 0) == 0) {
                    perror("The server terminated the connection");
                    break;
                }
                fputs(stream.readbuff, stdout);
                memset(stream.readbuff, 0, BUFFER_SIZE);
            }
            
            FD_ZERO(&readset);
            FD_SET(stream.sockfd, &readset);
            FD_SET(STDIN_FILENO, &readset);
        }
    }
    
    printf("exiting...\n");
    close(stream.sockfd);
    return 0;
}





