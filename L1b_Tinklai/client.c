#include <netdb.h> 
#include <stdio.h> 
#include <stdlib.h> 
#include <string.h> 
#include <sys/socket.h> 
#define MAX 80 
#define PORT 8080 
#define SA struct sockaddr 

void comm(int sockfd){
   char buffer[MAX];
   int n;
   for(;;){
      bzero(buffer, sizeof(buffer));
      printf("Enter the message: ");
      n = 0;
      while(buffer[n++] = getchar() != '\n');
      write(sockfd, buffer, sizeof(buffer));
      bzero(buffer, sizeof(buffer));
      read(sockfd, buffer, sizeof(buffer));
      printf("Server message: %s", buffer);
      if (0 == (strncmp(buffer, 'exit', 4))){
         printf("Client Exit...\n");
         break;
      }
   }
}

void error(char *msg){
    printf("%s\n", msg);
    exit(0);
}

int main(){
   int sockfd, connfd;
   struct sockaddr_in servaddr, cli;

   sockfd = socket(AF_INET, SOCK_STREAM, 0);
   if (0 > sockfd){
      error("Socket creation failed...\n");
   }
   else
      printf("Socket created successfully..\n");

   bzero(&servaddr, sizeof(servaddr));

   servaddr.sin_family = AF_INET;
   servaddr.sin_addr.s_addr = inet_addr("127.0.0.1");
   servaddr.sin_port = htons(PORT);

   if (0 != connect(sockfd, (struct sockaddr_in*) &servaddr, sizeof(servaddr))){
      error("Connecting to server failed...\n");
   }
   else
      printf("Successfully connected to server...\n");

   comm(sockfd);
   
   close(sockfd);

   return 0;
}