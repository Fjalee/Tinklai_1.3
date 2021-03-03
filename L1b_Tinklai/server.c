#include <stdio.h> 
#include <netdb.h> 
#include <netinet/in.h> 
#include <stdlib.h> 
#include <string.h> 
#include <sys/socket.h> 
#include <sys/types.h> 
#define MAX 80 
#define PORT 8080 //fix put 20000
#define SA struct sockaddr

void error(char *msg){
    printf("%s\n", msg);
    exit(0);
}

int main() {
    int sockfd, newsockfd, connfd, clilen;
    char buffer[256];
    struct sockaddr_in servaddr, cli_addr;
    int n;

    ////socket/////
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (0 > sockfd){
        printf("socket creation failed...\n");
        exit(0);
    }
    else
        printf("Socket successfully created...\n");

    bzero(&servaddr, sizeof(servaddr));
    ////socket//////


    /////assign IP, PORT/////
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = INADDR_ANY;
    servaddr.sin_port = htons(PORT);
    /////assign IP, PORT/////

    // /////Binding newly created socket to given IP and verification//////
    if (0 != (bind(sockfd, (SA*)&servaddr, sizeof(servaddr)))){
        printf("socket bind failed...\n");
        exit(0);
    }
    ///////Binding newly created socket to given IP and verification//////

    ///////////Listen///////////
    listen(sockfd, 5);
    clilen = sizeof(cli_addr);

    newsockfd= accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);

    if (newsockfd < 0){
        printf("accepting client failed...\n");
        exit(0);
    }

    bzero(buffer, 256);

    n = read(newsockfd, buffer, 255);

    if (0 > n){
        error("Reading failed...\n");
    }

    printf("Here is the message: %s\n", buffer);

    n = write(newsockfd, "I got your message", 18);

    if(n < 0){
        error("error writting");
    }


    ///////////Listen///////////


    
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);


   return 0;
}