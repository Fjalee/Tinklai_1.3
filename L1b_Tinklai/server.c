#include <stdio.h> 
#include <netdb.h> 
#include <netinet/in.h> 
#include <stdlib.h> 
#include <string.h> 
#include <sys/socket.h> 
#include <sys/types.h> 
#include <arpa/inet.h>
#include <unistd.h>
#include <ctype.h>
#define MAX 256
#define PORT 20000

void comm(int sockfd){
    char buffer[MAX];
    int n;

    bzero(buffer, MAX);
    read(sockfd, buffer, sizeof(buffer));

    printf("From client received: %s\n", buffer);

    for(int i=0; i<strlen(buffer); i++){
        buffer[i] = toupper(buffer[i]);
    }

    write(sockfd, buffer, sizeof(buffer));
}

void error(char *msg){
    printf("%s\n", msg);
    exit(0);
}

int main(int agrc, char *argv[]) {
    int sockfd, connfd, len;
    struct sockaddr_in servaddr, cliaddr;
    int n;

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (0 > sockfd)
        error("Socket creation failed...\n");
    else
        printf("Socket successfully created...\n");

    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(PORT);

    if (0 != (bind(sockfd, (struct sockaddr*)&servaddr, sizeof(servaddr))))
        error("Socket bind failed...\n");
    else 
        printf("Socket successfully binded...\n");

    if (0 != listen(sockfd, 5))
        error("Listen failed...\n");
    else
        printf("Server listening...\n");
    len = sizeof(cliaddr);

    connfd = accept(sockfd, (struct sockaddr *) &cliaddr, &len);
    if (connfd < 0)
        error("Server accept failed...\n");
    else
        printf("Server accepted successfully...\n");

    comm(connfd);

    close(sockfd);
}