 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#define PORT 8080

//compile cc MyServerUDP.c -o MyServerUDP
//run it as ./MyServerUDP

int main(int argc, char *argv[])
{
    struct sockaddr_in server, client;
 	int sockfd;
	char echoBuffer[256];
	 
    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
	{
          perror("socket() failed");  
		  exit(1);
    }


server.sin_family = AF_INET;                /* Internet address family */
server.sin_addr.s_addr = htonl(INADDR_ANY); /* Any incoming interface */
server.sin_port = htons(PORT);      
       
if (bind(sockfd, (struct sockaddr *) &server, sizeof(server)) < 0)
  {
  perror("bind() failed");
  exit(1);
  }
  
/* Run forever */
 while (1) 
{
   fprintf(stderr,"waiting ....\n");
   int len = sizeof(client); 
   if (recvfrom (sockfd, echoBuffer, sizeof(echoBuffer), 0, (struct sockaddr *)&client, &len)>=0)
   {
    fprintf( stderr, "I received: %s", echoBuffer);
    fprintf( stderr, "Length    : %d\n", strlen(echoBuffer)-1);
	fprintf( stderr, "--------------------------\n");
   }
}  
 close(sockfd); 
}