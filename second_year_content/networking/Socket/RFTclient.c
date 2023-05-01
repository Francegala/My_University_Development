#include "RFTheader.h"
#include <arpa/inet.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <stdbool.h>
#include <time.h> 
#include <string.h> 


/* error message format for fatalerror - Source Nick Cook*/
static const char *ERR_FMT = "%s:%d - %s, errno %d: %s\n";

/**************************************************************************
**************************************************************************
 All functions' signature
**************************************************************************
**************************************************************************/
int checksum(char *content, int len);
FILE * check_file(char *fileName);
int file_size(FILE *file);
void fatalerror(int line, char *msg);
bool isCorrupted(float prob);

struct sockaddr_in create_server_data(int sockfd , char* host, int port);
void send_meta_data(FILE *file,char *output, int sockfd, struct sockaddr_in server);
void send_file_normal(FILE *fp,int sockfd, struct sockaddr_in server);
void send_file_with_timeout(FILE *fp,int sockfd, struct sockaddr_in server, float prob_loss);

/**************************************************************************
**************************************************************************
 Functions that you need to implement
**************************************************************************
**************************************************************************/
/* arguments: socket id, host name, port number 
   return: The internet socket address of the server */ 
struct sockaddr_in create_server_data(int sockfd , char* host, int port)
{  
    struct sockaddr_in server; 
		server.sin_family = AF_INET;
		server.sin_addr.s_addr = inet_addr(host);
		server.sin_port = htons (port);
	return server;
} 
  
/* arguments: input file, the name of the output file, socket id, 
              port number, structure containing the server internet address */ 
void send_meta_data(FILE *file,char *output, int sockfd, struct sockaddr_in server)
{ 
 meta_data metadata; 
 metadata.size= file_size(file);
 strcpy(metadata.name, output);
 if(sendto(sockfd,&metadata,sizeof(metadata),0,(struct sockaddr *) &server, sizeof(server))<0){
	printf(" Error at sending the segment ! Exit the program safely \n");
	exit(1);
	}
} 
  
/* arguments: input file, socket id, internet socket address of the receiver  */
void send_file_normal(FILE *fp,int sockfd, struct sockaddr_in server)
{
	printf("SENDER: Start Sending File\n");
	printf("--------------------------\n");

	//open file
	fp = fopen("input.txt", "r+");
	
	//initialise memory and variables
	char* code = (char*)malloc(15*sizeof(char));
    size_t n = 0;
    int c;
	int countChar = 0;
	int len = sizeof(server);
	int value = 1;
	int counter = 0;

    if (fp == NULL){
		printf(" Cannot open the file ! Exit the program safely \n");
		exit(1);}
		
	// until chars finished
    while ((c = fgetc(fp)) != EOF)
    {
		//read the char
        code[n++] = (char) c;
		countChar++;
		if(countChar == 15){
		//create segments
		segment segmentToSend;
		segment segmentToBeReceived;
		segmentToSend.size = n;
		//set right sequence number
		if(value == 0){	segmentToSend.sq = 1;}
		else{segmentToSend.sq = 0;}
		//copy content and assign checksum
		segmentToSend.type = TYPE_DATA;
		strncpy(segmentToSend.payload, code, segmentToSend.size+1);
		segmentToSend.checksum = checksum(segmentToSend.payload, segmentToSend.size);
		//send segment
		if(sendto(sockfd,&segmentToSend,sizeof(segmentToSend),0,(struct sockaddr *) &server, sizeof(server))<0){
		printf(" Error at sending the segment ! Exit the program safely \n");
		exit(1);
		}
		counter++;
		printf("SENDER: Sending segment: sq: %d, size:%d, checksum:%d, content:%s \n",
					segmentToSend.sq,segmentToSend.size,segmentToSend.checksum,segmentToSend.payload);
		printf("SENDER: Waiting for an ack\n");
		//receive ACK
		if(recvfrom(sockfd,&segmentToBeReceived,sizeof(segmentToBeReceived),0, (struct sockaddr *)&server, &len)<0){
		printf(" Error at receiving the segment! Exit the program safely \n");
		exit(1);
		}	
		if(segmentToBeReceived.sq != segmentToSend.sq){
			printf(" Sequence numbers not corresponding! Exit the program safely \n");
			exit(1);
		}
		printf("SENDER: ACK sq=%d RECEIVED \n", segmentToBeReceived.sq);
		value = segmentToBeReceived.sq;
		printf("--------------------------\n");
		
		//start again
		countChar = 0;
		n = 0;
	    free(code);
		code = (char*)malloc(15*sizeof(char));
		}
    }

    // don't forget to terminate with the null character
	code[n] = '\0';    

	//send last segment 
		segment segmentToSend;
		segment segmentToBeReceived;
		segmentToSend.size = n;
		if(value == 0){	segmentToSend.sq = 1;}
		else{segmentToSend.sq = 0;}
		segmentToSend.type = TYPE_DATA;
		strncpy(segmentToSend.payload, code, segmentToSend.size+1);
		segmentToSend.checksum = checksum(segmentToSend.payload, segmentToSend.size);
		if(sendto(sockfd,&segmentToSend,sizeof(segmentToSend),0,(struct sockaddr *) &server, sizeof(server))<0){
		printf(" Error at sending the segment ! Exit the program safely \n");
		exit(1);
		}
		counter++;
		printf("SENDER: Sending segment: sq: %d, size:%d, checksum:%d, content:%s \n",
					segmentToSend.sq,segmentToSend.size,segmentToSend.checksum,segmentToSend.payload);
		printf("SENDER: Waiting for an ack\n");
		if(recvfrom(sockfd,&segmentToBeReceived,sizeof(segmentToBeReceived),0, (struct sockaddr *)&server, &len)<0){
		printf(" Error at receiving the segment! Exit the program safely \n");
		exit(1);
		}	
		if(segmentToBeReceived.sq != segmentToSend.sq){
			printf(" Sequence numbers not corresponding! Exit the program safely \n");
			exit(1);
		}
		printf("SENDER: ACK sq=%d RECEIVED \n", segmentToBeReceived.sq);
		value = segmentToBeReceived.sq;
		printf("--------------------------\n");
		
		//free memory		
	    free(code);
	
printf("Total number of sent segments: %d \n",counter);
fclose(fp);	   
}


 /* arguments: input file, socket id, internet socket address of the receiver  */
void send_file_with_timeout(FILE *fp,int sockfd, struct sockaddr_in server, float prob_loss)
{ 

printf("SENDER: Start Sending File\n");
	printf("--------------------------\n");
	// open file and initialise memory and variables
	fp = fopen("input.txt", "r+");
	
	char* code = (char*)malloc(15*sizeof(char));
    size_t n = 0;
    int c;
	int countChar = 0;
	int len = sizeof(server);
	int value = 1;
	int counter = 0;
	int selectValue;
	struct timeval timeout;
	timeout.tv_sec = 5;
	timeout.tv_usec = 0;
	fd_set readfds;
	

    if (fp == NULL){
		printf(" Cannot open the file ! Exit the program safely \n");
		exit(1);}
		
	// read chars one by one
    while ((c = fgetc(fp)) != EOF)
    {
        code[n++] = (char) c;
		countChar++;
		if(countChar == 15){
		//create segments with right content 
		segment segmentToSend;
		segment segmentToBeReceived;
		segmentToSend.size = n;
		if(value == 0){	segmentToSend.sq = 1;}
		else{segmentToSend.sq = 0;}
		segmentToSend.type = TYPE_DATA;
		strncpy(segmentToSend.payload, code, segmentToSend.size+1);
		// use isCorrupted mathod to assign checksum
		if(isCorrupted(prob_loss)==false){
			segmentToSend.checksum = checksum(segmentToSend.payload, segmentToSend.size);}
		else {
			segmentToSend.checksum = 0;}
		// send it
		if(sendto(sockfd,&segmentToSend,sizeof(segmentToSend),0,(struct sockaddr *) &server, sizeof(server))<0){
		printf(" Error at sending the segment ! Exit the program safely \n");
		exit(1);
		}
		counter++;
		printf("SENDER: Sending segment: sq: %d, size:%d, checksum:%d, content:%s \n",
					segmentToSend.sq,segmentToSend.size,segmentToSend.checksum,segmentToSend.payload);
		if(segmentToSend.checksum ==0){printf(">>>>>> Network: segment checksum is corrupted <<<<<<\n");}
		printf("SENDER: Waiting for an ack\n");
		
		//ask for ack select and timeout
		FD_ZERO(&readfds);
		FD_SET(sockfd,&readfds);
		
		//first number int nfds what should I put? <- ?
		selectValue = select(5, &readfds, NULL, NULL, &timeout);
		
		// if select timeouts resend assigning new checksum
		while(selectValue == 0){
			printf("\n TIMEOUT: Re-sending the same segment \n \n");
						
			if(isCorrupted(prob_loss)==false){
				segmentToSend.checksum = checksum(segmentToSend.payload, segmentToSend.size);}
			else {
				segmentToSend.checksum = 0;}

			if(sendto(sockfd,&segmentToSend,sizeof(segmentToSend),0,(struct sockaddr *) &server, sizeof(server))<0){
				printf(" Error at sending the segment ! Exit the program safely \n");
				exit(1);
			}
			counter++;
			printf("SENDER: Sending segment: sq: %d, size:%d, checksum:%d, content:%s \n",
					segmentToSend.sq,segmentToSend.size,segmentToSend.checksum,segmentToSend.payload);
			if(segmentToSend.checksum ==0){printf(">>>>>> Network: segment checksum is corrupted <<<<<<\n");}
			printf("SENDER: Waiting for an ack \n");
		
			//ask for ack select
			FD_ZERO(&readfds);
			FD_SET(sockfd,&readfds);
			
		//first number int nfds what should I put? <-
			selectValue = select(5, &readfds, NULL, NULL, &timeout);
		}
		// wait for an ACK
		if(recvfrom(sockfd,&segmentToBeReceived,sizeof(segmentToBeReceived),0, (struct sockaddr *)&server, &len)<0){
		printf(" Error at receiving the segment! Exit the program safely \n");
		exit(1);
		}	
		if(segmentToBeReceived.sq != segmentToSend.sq){
			printf(" Sequence numbers not corresponding! Exit the program safely \n");
			exit(1);
		}
		printf("SENDER: ACK sq=%d RECEIVED \n", segmentToBeReceived.sq);
		value = segmentToBeReceived.sq;
		printf("--------------------------\n");
		
		//start again
		countChar = 0;
		n = 0;
	    free(code);
		code = (char*)malloc(15*sizeof(char));
		}
    }

    // don't forget to terminate with the null character
	code[n] = '\0';    

	//send last segment 
		segment segmentToSend;
		segment segmentToBeReceived;
		segmentToSend.size = n;
		if(value == 0){	segmentToSend.sq = 1;}
		else{segmentToSend.sq = 0;}
		segmentToSend.type = TYPE_DATA;
		strncpy(segmentToSend.payload, code, segmentToSend.size+1);
		
		if(isCorrupted(prob_loss)==false){
			segmentToSend.checksum = checksum(segmentToSend.payload, segmentToSend.size);}
		else {
			segmentToSend.checksum = 0;}
			
		if(sendto(sockfd,&segmentToSend,sizeof(segmentToSend),0,(struct sockaddr *) &server, sizeof(server))<0){
		printf(" Error at sending the segment ! Exit the program safely \n");
		exit(1);
		}
		counter++;
		printf("SENDER: Sending segment: sq: %d, size:%d, checksum:%d, content:%s \n",
					segmentToSend.sq,segmentToSend.size,segmentToSend.checksum,segmentToSend.payload);
		if(segmentToSend.checksum ==0){printf(">>>>>> Network: segment checksum is corrupted <<<<<<\n");}
		printf("SENDER: Waiting for an ack\n");
		

		////////////////////////// FIX if last segment is lost start //////////////////////////
		
				
		//ask for ack select and timeout
		FD_ZERO(&readfds);
		FD_SET(sockfd,&readfds);
		
		//first number int nfds what should I put? <- ?
		selectValue = select(5, &readfds, NULL, NULL, &timeout);
		
		// if select timeouts resend assigning new checksum
		while(selectValue == 0){
			printf("\n TIMEOUT: Re-sending the same segment \n \n");
						
			if(isCorrupted(prob_loss)==false){
				segmentToSend.checksum = checksum(segmentToSend.payload, segmentToSend.size);}
			else {
				segmentToSend.checksum = 0;}

			if(sendto(sockfd,&segmentToSend,sizeof(segmentToSend),0,(struct sockaddr *) &server, sizeof(server))<0){
				printf(" Error at sending the segment ! Exit the program safely \n");
				exit(1);
			}
			counter++;
			printf("SENDER: Sending segment: sq: %d, size:%d, checksum:%d, content:%s \n",
					segmentToSend.sq,segmentToSend.size,segmentToSend.checksum,segmentToSend.payload);
			if(segmentToSend.checksum ==0){printf(">>>>>> Network: segment checksum is corrupted <<<<<<\n");}
			printf("SENDER: Waiting for an ack \n");
		
			//ask for ack select
			FD_ZERO(&readfds);
			FD_SET(sockfd,&readfds);
			
		//first number int nfds what should I put? <-
			selectValue = select(5, &readfds, NULL, NULL, &timeout);
		}
		
		
		////////////////////////// FIX if last segment is lost end //////////////////////////
		
		
		if(recvfrom(sockfd,&segmentToBeReceived,sizeof(segmentToBeReceived),0, (struct sockaddr *)&server, &len)<0){
		printf(" Error at receiving the segment! Exit the program safely \n");
		exit(1);
		}	
		if(segmentToBeReceived.sq != segmentToSend.sq){
			printf(" Sequence numbers not corresponding! Exit the program safely \n");
			exit(1);
		}
		printf("SENDER: ACK sq=%d RECEIVED \n", segmentToBeReceived.sq);
		value = segmentToBeReceived.sq;
		printf("--------------------------\n");
		
		//free memory		
	    free(code);
	
    
	
	
printf("Total number of sent segments: %d \n",counter);
fclose(fp);	   

}

/**************************************************************************
**************************************************************************
                  The main function  
**************************************************************************
**************************************************************************/
int main(int argc,char *argv[])
{
	int sockfd ;
	FILE *file;
    struct sockaddr_in server;
	
	/* 	accept input from console in the form of 
	./client inputFile outputFile localhost portNumber 	*/
	if (argc != 5) {
		fprintf(stderr, "Usage: <inputFile> <outputFile> <localhost> <portNumber> \n");
		exit(1);
	}
		
	/* check the file actually exist	*/
	file=check_file(argv[1]);
	printf ("----------------------------------------------------\n");
	printf ("SENDER: File (%s) exists with a size of (%d) bytes\n",argv[1], file_size(file));
	printf ("----------------------------------------------------\n");
	printf ("----------------------------------------------------\n");

	/* create a socket */
	sockfd =socket(AF_INET,SOCK_DGRAM,0);
	if(sockfd <0)
	{
		perror("SENDER: Failed to open socket"); 
		close (sockfd );
		exit(1);
	}
	printf ("SENDER: Socket is created\n");

	/* create a connection */
	server = create_server_data(sockfd, argv[3], atoi(argv[4]));
 	printf ("SENDER: Server data is prepared\n");
 	
	/* Send meta data */
	send_meta_data(file,argv[2],sockfd,server);
	
	/* transmission options */
	int choice=0;
    float loss=0;
	printf ("----------------------------------------------------\n");
	printf ("----------------------------------------------------\n");
	printf("Choose one of the following options (1 or 2):\n ");
	printf("1. Normal transmission (no data segment is lost) \n ");
	printf("2: Transmission with time-out capabilities\n ");
    scanf("%d", &choice);
    switch(choice)
    {
        case 1:
            send_file_normal(file,sockfd, server);
            break;
        
        case 2:
			printf("Enter the probability of a corrupted checksum (between 0 and 1):\n ");
			scanf("%f", &loss);
			send_file_with_timeout(file,sockfd, server, loss);
            break;

         default:
            printf("Error! enter 1 or 2 \n");
    } 

	printf("SENDER: File is sent\n");
	
	/* Close the file */
	fclose(file);
	
	/* Close the socket */
	close (sockfd );
	
 	return 0;
} 

/*************************************************************************
**************************************************************************
 These functions are implemented for you .. Do NOT Change them
**************************************************************************
**************************************************************************/


/* calculate the segment checksum by adding the payload */
int checksum(char *content, int len)
{
	int i;
	int sum = 0;
	for (i = 0; i < len; i++)
		sum += (int)(*content++);
	return sum;
}

/* check if the input file does exist */
FILE * check_file(char *fileName)
{
	FILE *file = fopen(fileName, "rb");
	if(!file) {
		perror("SENDER: File does not exists");
 		fclose(file);
		exit(1);
	}
	return file;
}

/* return file size */
int file_size(FILE *file)
{
	fseek(file, 0L, SEEK_END);
	int size = ftell(file);
	return size;
}

void fatalerror(int line, char *msg) {
    printf("Line %d : %s\n", line, msg);
    exit(0);
}

/* decide whether the segment is corrupted or not given a certain probability */
bool isCorrupted(float prob)
{
    srand(time(NULL)); 
    if(((float)(rand())/(float)(RAND_MAX)) < prob)
	{
	return true;
	}
    else return false;
}
/*************************************************************************** 
 *************************************************************************** 
 *************************************************************************** 
 ***************************************************************************/

