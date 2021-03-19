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

#define PORT "20000"
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
    int bs_sockfd, bs_clientfd = 0;
    int ro_cli_sockfd;    //read-only client
    int cli_sockfd;    //regular client


    if (-1 == (bs_sockfd = createSocket(PORT_BS))){
        if (-1 != (bs_clientfd = connectToSndServer()))
            printf("Successfully connected to another server...\n");
        else
            error("Binding socket for server failed ...\n");
    }
    else{
        printf("Successfully binded BS...\n");

        if (listen(bs_sockfd, BACKLOG) == 0)
            printf("Listening for another server...\n");
        
        bs_addr_size = sizeof bs_cli_addr;
        bs_sockfd = accept(bs_sockfd, (struct sockaddr *)&bs_cli_addr, &bs_addr_size);
        printf("Successfully accepted another server...\n");
    }

    ro_cli_sockfd = listenForClient(argv[1]);
    cli_sockfd = listenForClient(argv[2]);

    //comm(new_fd);

    char buff[MAXLEN];
    bzero(buff, MAXLEN);
    recv(cli_sockfd, buff, MAXLEN, 0);

    printf("Received: %s\n", buff);
    printf("Forwarding message: %s\n", buff);
    send(ro_cli_sockfd, buff, MAXLEN, 0);


    close(bs_sockfd);
    close(ro_cli_sockfd);
    close(cli_sockfd);
    
    printf("\n");
    return 0;
}























