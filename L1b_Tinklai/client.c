#include <stdio.h> 
#include <netdb.h> 
#include <netinet/in.h> 
#include <stdlib.h> 
#include <string.h> 
#include <sys/socket.h> 
#include <sys/types.h> 
#include <arpa/inet.h>
#include <unistd.h>

#define MAX 256
#define PORT "20000"
#define BACKLOG 10

void error(char *msg){
    printf("%s\n", msg);
    exit(0);
}

void comm(int sockfd){
    char *buff;
    int len;

    printf("Send: ");
    bzero(buff, MAX);
    scanf("%s", buff);

    if (send(sockfd, buff, sizeof(buff), 0) < 0)
        error("Sending failed...");

    recv(sockfd, buff, sizeof(buff), 0);
    printf("Received: %s\n", buff);
}

int main(/*int agrc, char **argv*/){
    struct sockaddr_storage cli_addr;
    socklen_t addr_size;
    struct addrinfo hints, *servinfo, *i;
    int sockfd, new_fd;

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;
    getaddrinfo("127.0.0.1"/*argv[1]*/, PORT, &hints, &servinfo);

    for (i = servinfo; i != NULL; i = i->ai_next) {
        sockfd = socket(i->ai_family, i->ai_socktype, i->ai_protocol);
        if (sockfd == -1)
            continue;

        if (-1 != connect(sockfd, i->ai_addr, i->ai_addrlen))
            break;
    }
    freeaddrinfo(servinfo);

    if (i == NULL)
        error("Connecting failed...\n");
    else
        printf("Successfully connected...\n");

    comm(sockfd);

    close(sockfd);
    
    printf("\n");
    return 0;
}