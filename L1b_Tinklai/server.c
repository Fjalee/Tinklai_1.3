#include <stdio.h> 
#include <ctype.h>
#include <netdb.h> 
#include <netinet/in.h> 
#include <stdlib.h> 
#include <string.h> 
#include <sys/socket.h> 
#include <sys/types.h> 
#include <arpa/inet.h>
#include <unistd.h>
#include <poll.h>
#include <sys/wait.h>
#include <signal.h>

#define PORT_BS "20082"    //port between servers
#define BACKLOG 10
#define MAXLEN 10000

int createSocket(char *port){
    struct addrinfo hints, *servinfo, *i;
    int sockfd;

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_INET6;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;
    getaddrinfo(NULL, port, &hints, &servinfo);
    
    for (i = servinfo; i != NULL; i = i->ai_next) {
        sockfd = socket(i->ai_family, i->ai_socktype, i->ai_protocol);
        if (sockfd == -1)
            continue;

        if (bind(sockfd, i->ai_addr, i->ai_addrlen) == 0)
            break;
    }
    freeaddrinfo(servinfo);

    if (i == NULL)
        return -1;         //binding failed, ether something went wrong or port taken by another server
    else
        return sockfd;
}

int listenForClient(char *port){
    struct sockaddr_storage cli_addr;
    socklen_t addr_size;
    int sockfd, new_fd;
    int x=1;

    if (-1 == (sockfd = createSocket(port))){
        printf("Binding failed on port %s...\n", port);
        exit(0);
    }
    else
        printf("Successfully binded on port %s...\n", port);


    if (listen(sockfd, BACKLOG) == 0)
        printf("Listening for clients on port %s...\n", port);

    addr_size = sizeof cli_addr;
    new_fd = accept(sockfd, (struct sockaddr *)&cli_addr, &addr_size);
    printf("Successfully accepted on port %s...\n", port);

    return new_fd;
}

int connectToSndServer(){
    struct sockaddr_storage cli_addr;
    socklen_t addr_size;
    struct addrinfo hints, *servinfo, *i;
    int sockfd, new_fd;

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;
    getaddrinfo("::1", PORT_BS, &hints, &servinfo);

    for (i = servinfo; i != NULL; i = i->ai_next) {
        sockfd = socket(i->ai_family, i->ai_socktype, i->ai_protocol);
        if (sockfd == -1)
            continue;

        if (-1 != connect(sockfd, i->ai_addr, i->ai_addrlen))
            break;
    }
    freeaddrinfo(servinfo);

    if (i == NULL)
        return -1;
    else
        return sockfd;
}

void error(char *msg){
    printf("%s\n", msg);
    exit(0);
}

int main(int agrc, char *argv[]){
    struct sockaddr_storage cli_addr, bs_cli_addr;
    socklen_t addr_size, bs_addr_size;
    struct addrinfo hints, *servinfo, *i;
    int bs_sockfd, sockfd;
    int ro_cli_sockfd;    //read-only client
    int cli_sockfd;    //regular client
    char buff[MAXLEN];

    int fd_count = 0;
    int fd_size = 5;
    struct pollfd *pfds = malloc(sizeof *pfds * fd_size);

    char *cli_port = argv[1], *ro_cli_port = argv[2];


    if (-1 == (sockfd = createSocket(PORT_BS))){
        if (-1 != (bs_sockfd = connectToSndServer())){
            printf("Successfully connected to another server...\n");
        }
        else
            error("Binding socket for server failed ...\n");
    }
    else{
        printf("Successfully binded BS...\n");

        if (listen(sockfd, BACKLOG) == 0)
            printf("Listening for another server...\n");
        
        bs_addr_size = sizeof bs_cli_addr;
        bs_sockfd = accept(sockfd, (struct sockaddr *)&bs_cli_addr, &bs_addr_size);
        printf("Successfully accepted another server...\n");

        pfds[0].fd = bs_sockfd;
        pfds[0].events = POLLIN;
        fd_count = 1;      
    }

    ro_cli_sockfd = listenForClient(ro_cli_port);
    cli_sockfd = listenForClient(cli_port);

    // ro_cli_sockfd = 10;
    // cli_sockfd = 11;

    pfds[0].fd = bs_sockfd;
    pfds[0].events = POLLIN;
    fd_count = 1;

    pfds[fd_count].fd = cli_sockfd;
    pfds[fd_count].events = POLLIN;
    fd_count++;

    for(;;){
        int poll_count = poll(pfds, fd_count, -1);
        if (poll_count == -1)
            error("Failed creating poll...\n");

        for(int i=0; i<fd_count; i++){
            if (pfds[i].revents & POLLIN) {
                if (bs_sockfd == pfds[i].fd){
                    bzero(buff, MAXLEN);
                    int nbytes = recv(bs_sockfd, buff, sizeof buff, 0);
                    printf("Received: %s", buff);

                    printf("Forwarding message to read-only client: %s\n", buff);
                    send(ro_cli_sockfd, buff, nbytes, 0);
                }
                else if (cli_sockfd == pfds[i].fd){
                    bzero(buff, MAXLEN);
                    int nbytes = recv(cli_sockfd, buff, sizeof buff, 0);
                    printf("Received: %s", buff);

                    printf("Forwarding message to read-only client: %s", buff);
                    send(ro_cli_sockfd, buff, nbytes, 0);
                    printf("Forwarding message to another server: %s\n", buff);
                    send(bs_sockfd, buff, nbytes, 0);
                }
            }
        }
    }


    close(bs_sockfd);
    close(ro_cli_sockfd);
    close(cli_sockfd);
    
    printf("\n");
    return 0;
}























