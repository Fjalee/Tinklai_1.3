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
#define PORT 8080 //fix put 20000
#define SA struct sockaddr

void comm(int sockfd){
    char buffer[MAX];
    int n;

    bzero(buffer, MAX);

    read(sockfd, buffer, sizeof(buffer));

    printf("From client received: %s\n", buffer);
    bzero(buffer, MAX);
    n = 0;

    printf("Enter the message: ");
    while ((buffer[n++] = getchar()) != '\n');
    write(sockfd, buffer, sizeof(buffer));
}

void error(char *msg){
    printf("%s\n", msg);
    exit(0);
}

int main() {
    int sockfd, connfd, len;
    struct sockaddr_in servaddr, cli;
    int n;

    ////socket/////
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (0 > sockfd){
        printf("Socket creation failed...\n");
        exit(0);
    }
    else
        printf("Socket successfully created...\n");

    bzero(&servaddr, sizeof(servaddr));
    ////socket//////


    /////assign IP, PORT/////
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(PORT);
    /////assign IP, PORT/////

    // /////Binding newly created socket to given IP and verification//////
    if (0 != (bind(sockfd, (SA*)&servaddr, sizeof(servaddr)))){
        error("Socket bind failed...");
    }
    else 
        printf("Socket successfully binded...\n");
    ///////Binding newly created socket to given IP and verification//////

    ///////////Listen///////////
    if (0 != listen(sockfd, 5)){
        error("Listen failed...\n");
    }
    else
        printf("Server listening...\n");
    len = sizeof(cli);

    connfd = accept(sockfd, (struct sockaddr *) &cli, &len);
    if (connfd < 0){
        error("Server accept failed...\n");
    }
    else
        printf("Server accepted successfully...\n");

    comm(connfd);

    close(sockfd);
}