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
#define PORT_OUT "22222"    //port for output only client
#define BACKLOG 10
#define MAXLEN 10000

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

void strToUpper(char *str){
    for (int i; i <= strlen(str); i++){
        str[i] = toupper(str[i]);
    }
}

void comm(int clifd){
    char buff[MAXLEN];
    bzero(buff, MAXLEN);
    recv(clifd, buff, MAXLEN, 0);

    printf("Received: %s\n", buff);
    strToUpper(buff);
    printf("Sending: %s\n", buff);

    send(clifd, buff, MAXLEN, 0);
}

void error(char *msg){
    printf("%s\n", msg);
    exit(0);
}

int main(int agrc, char *argv[]){
    struct sockaddr_storage cli_addr;
    socklen_t addr_size;
    struct addrinfo hints, *servinfo, *i;
    int sockfd, new_fd, im_clientfd = 0;




    if (-1 == (sockfd = createSocket(PORT_BS))){
        if (-1 != (im_clientfd = connectToSndServer()))
            printf("Successfully connected to another server...\n");
        else
            error("Binding failed...\n");
    }
    else
        printf("Successfully binded...\n");


    if (listen(sockfd, BACKLOG) == 0)
        printf("Listening...\n");

    addr_size = sizeof cli_addr;
    new_fd = accept(sockfd, (struct sockaddr *)&cli_addr, &addr_size);
    printf("Successfully accepted another server...\n");

    comm(new_fd);

    close(sockfd);
    close(new_fd);
    
    printf("\n");
    return 0;
}























