#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 
#include <string.h>
  #include <arpa/inet.h>
#include <stdio.h>
//compile cc MyClientUDP.c -o MyClientUDP
//run it as ./MyClientUDP localhost 8080
//OR
//./MyClientUDP 127.0.0.1 8080

int main(int argc, char *argv[])
{
struct sockaddr_in  server;
int sockfd;
char echoString[256] ;
struct hostent *lh;

if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
{
  perror("socket() failed");
  exit(1);
}

server.sin_family = AF_INET;            /* Internet address family */
server.sin_port = htons(atoi(argv[2])); /* Server port */

/* gethostbyname() returns a pointer to a hostent struct or NULL.    */
lh=gethostbyname(argv[1]); 
  if (!lh) {
       printf("%s was not resolved\n",argv[1]);
      exit(1);
    }
	
memcpy(&server.sin_addr,lh->h_addr,lh->h_length);
// or server.sin_addr.s_addr = inet_addr(argv[1]); 

//run until the client enters .
while (strncmp(echoString,".",1)!=0){
  printf("Please enter the message: ");
  fgets(echoString,255,stdin);
  if (sendto(sockfd,&echoString,sizeof(echoString),0,(struct sockaddr *)&server, sizeof(server))<=0)
	{
       perror("sendto failed");
       exit(1);
     }
}
close(sockfd); 
}