#include <netdb.h> 
#include <stdio.h> 
#include <stdlib.h> 
#include <string.h> 
#include <sys/socket.h> 
#include <arpa/inet.h>
#include <unistd.h>
#define MAX 256
#define PORT 20000

void comm(int sockfd){
   char buffer[MAX];
   int n;

   printf("Enter the message: ");
   n = 0;
   bzero(buffer, sizeof(buffer));
   while((buffer[n++] = getchar()) != '\n');
   write(sockfd, buffer, sizeof(buffer));

   bzero(buffer, sizeof(buffer));
   read(sockfd, buffer, sizeof(buffer));
   printf("Server message: %s\n", buffer);
}

void error(char *msg){
    printf("%s\n", msg);
    exit(1);
}

int main(int agrc, char *argv[]){
   int sockfd, connfd;
   struct sockaddr_in servaddr;

   sockfd = socket(AF_INET, SOCK_STREAM, 0);
   if (0 > sockfd)
      error("Socket creation failed...\n");
   else
      printf("Socket created successfully..\n");

   bzero(&servaddr, sizeof(servaddr));
   servaddr.sin_family = AF_INET6;
   servaddr.sin_addr.s_addr = inet_addr("::1");
   servaddr.sin_port = htons(PORT);

   if (0 != connect(sockfd, (struct sockaddr*) &servaddr, sizeof(servaddr)))
      error("Connecting to server failed...\n");
   else
      printf("Successfully connected to server...\n");

   comm(sockfd);

   close(sockfd);
}