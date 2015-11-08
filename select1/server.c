//
//  server.c
//  serv
//
//  Created by Ben Oliver on 08/11/15.
//  Copyright © 2015 Ben Oliver. All rights reserved.
//

#include <stdlib.h> // exit
#include <stdio.h>
#include <errno.h>

#include <sys/types.h>
#include <netinet/in.h> // (struct sockaddr_in),
#include <sys/socket.h> // socket, listen
#include <string.h>     // memset,
#include <netdb.h>
#include <netinet/in.h>
#include <sys/uio.h>
#include <unistd.h>
#include <arpa/inet.h> // inet_ntoa, ntohs
#include <assert.h>
#include <inttypes.h>


#define BUFFER_SIZE 512
#define STANDARD_PROTOCOL 0
#define BACKLOG 10


static int str_to_uint16(const char *str, uint16_t *res) {
    char *end;
    errno = 0;
    intmax_t val = strtoimax(str, &end, 10);
    if (errno == ERANGE || val < 0 || val > UINT16_MAX || end == str || *end != '\0')
        return 0;
    *res = (uint16_t) val;
    return 1;
}


ssize_t read_from_connected_client (int fd, char buffer[BUFFER_SIZE]) {
    
    errno = 0;
    
    ssize_t nbytes = read(fd, buffer, BUFFER_SIZE);
    
    switch (nbytes) {
        case -1 :
            perror ("read");
            return -1;
        case 0 :
            // end of file
            return 0;
        default:
            return nbytes;
    }
}




int main(int argc, const char* argv[]) {
    
    if (argc != 2) {
        printf("<port>\n");
        exit(EXIT_FAILURE);
    }
    
    uint16_t PORT;
    if (!str_to_uint16(argv[1], &PORT)) {
        printf("invalid <port>\n");
        exit(EXIT_FAILURE);
    }
        
    char buffer[BUFFER_SIZE];
    
    int sockfd = socket(PF_INET, SOCK_STREAM, STANDARD_PROTOCOL);
    
//    socket() creates an endpoint for communication and returns a descriptor.
    
    struct sockaddr_in serv_addr;
    memset(&serv_addr, '0', sizeof(serv_addr));
    
    serv_addr.sin_family      = AF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_addr.sin_port        = htons(PORT);

//    INADDR_ANY allows program to work without knowing the IP address of the machine it was running on,
//    or, in the case of a machine with multiple network interfaces, it allows the server
//    to receive packets destined to any of the interfaces.

    bind(sockfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr));
    
//    bind() assigns a name to an unnamed socket.  When a socket is created with
//    socket(2) it exists in a name space (address family) but has no name
//    assigned.  bind() requests that address be assigned to the socket.
    
    listen(sockfd, BACKLOG);
    
//    Creation of socket-based connections requires several operations.  First, a
//    socket is created with socket(2).  Next, a willingness to accept incoming
//    connections and a queue limit for incoming connections are specified with
//    listen().  Finally, the connections are accepted with accept(2).  The
//    listen() call applies only to sockets of type SOCK_STREAM.
//        
//    The backlog parameter defines the maximum length for the queue of pending
//    connections.  If a connection request arrives with the queue full, the
//    client may receive an error with an indication of ECONNREFUSED.  Alterna-
//    tively, if the underlying protocol supports retransmission, the request may
//    be ignored so that retries may succeed.
    
    fd_set active_fd_set, read_fd_set;
    struct sockaddr_in clientname;
    
    FD_ZERO (&active_fd_set);
    FD_SET  (sockfd, &active_fd_set);
    
    while (1) {
        
        memset(buffer, 0, BUFFER_SIZE);
        
        read_fd_set = active_fd_set;
        
        if (select (FD_SETSIZE, &read_fd_set, NULL, NULL, NULL) < 0) {
            perror ("select");
            exit (EXIT_FAILURE);
        }

        for (int fd = 0; fd < FD_SETSIZE; ++fd) {
            if (FD_ISSET(fd, &read_fd_set)) {
                if (fd == sockfd) {
                    
//                    connection on server socket
//                    we want to get the address of the client connecting
//                    and add its fd to the set of connections
                    
                    printf("something is trying to connect\n");
                    socklen_t size = sizeof (clientname);
                    int new_connfd = accept(sockfd, (struct sockaddr*) &clientname, &size);
                    
//                    The argument socket is a socket that has been created with socket(2), bound
//                    to an address with bind(2), and is listening for connections after a
//                    listen(2).  accept() extracts the first connection request on the queue of
//                    pending connections, creates a new socket with the same properties of
//                    socket, and allocates a new file descriptor for the socket.  If no pending
//                    connections are present on the queue, and the socket is not marked as non-
//                    blocking, accept() blocks the caller until a connection is present.  If the
//                    socket is marked non-blocking and no pending connections are present on the
//                    queue, accept() returns an error as described below.  The accepted socket
//                    may not be used to accept more connections.  The original socket socket,
//                    remains open.
                    
                    if (new_connfd < 0) {
                        perror ("accept");
                        exit (EXIT_FAILURE);
                    }
                    
                    assert(ntohs(clientname.sin_port) > 0);
                    
                    fprintf (stderr,"Server: connect from host %s, port %d.\n", inet_ntoa(clientname.sin_addr), ntohs(clientname.sin_port));
                    
                    FD_SET (new_connfd, &active_fd_set);
                
                } else {
                    
//                    connection on some existing client socket
                    ssize_t nbytes;
                    if ((nbytes = read_from_connected_client(fd, buffer)) < 1) {
                        memset(buffer, 0, BUFFER_SIZE);
                        fflush(stderr);
                        close(fd);
                        FD_CLR(fd, &active_fd_set);
                    } else {
                        fflush(stderr);
                        fprintf (stderr, "Server: got message: %s", buffer);
                        fflush(stderr);
                        memset(buffer, 0, BUFFER_SIZE);
                    }
                }
            }
        }
    }
    
    return 0;
}























