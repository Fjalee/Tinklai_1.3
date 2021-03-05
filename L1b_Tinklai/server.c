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

void comm(int clifd){

}

void error(char *msg){
    printf("%s\n", msg);
    exit(0);
}

int main(){
    struct sockaddr_storage cli_addr;
    socklen_t addr_size;
    struct addrinfo hints, *servinfo, *i;
    int sockfd, new_fd;

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;
    getaddrinfo(NULL, PORT, &hints, &servinfo);

    

    for (i = servinfo; i != NULL; i = i->ai_next) {
        sockfd = socket(i->ai_family, i->ai_socktype, i->ai_protocol);
        if (sockfd == -1)
            continue;

        if (bind(sockfd, i->ai_addr, i->ai_addrlen) == 0)
            break;
    }
    freeaddrinfo(servinfo);
    if (i == NULL)
        error("Bind failed...\n");
    else
        printf("Successfully binded...\n");


    if (listen(sockfd, BACKLOG) == 0)
        printf("Listening...\n");

    addr_size = sizeof cli_addr;
    new_fd = accept(sockfd, (struct sockaddr *)&cli_addr, &addr_size);
    printf("Successfully accepted...\n");


    close(sockfd);
    
    return 0;
}























