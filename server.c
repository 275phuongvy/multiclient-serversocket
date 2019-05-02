#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <sys/types.h>

#define PORT 4444
#define BUF_SIZE 1024

int mergesort(char list[20][BUF_SIZE], int length);
int merge(char list[20][BUF_SIZE], int left_start, int left_end, int right_start, int right_end);
int mergesort_r(int left, int right, char list[20][BUF_SIZE]);

int main()
{
    
	int sockfd, ret;
	struct sockaddr_in serverAddr; // Local address

	int newSocket;
	struct sockaddr_in newAddr;

	socklen_t addr_size;
	pid_t childpid;
	unsigned int childProcCount =0; //number of child processes

	// Create socket for incoming connections
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if(sockfd < 0){
		printf("Error in connection.\n");
		exit(1);
	}
	printf("Server Socket is created.\n");

	// Construct local address structure
	memset(&serverAddr, '\0', sizeof(serverAddr));        // Zero out structure
	serverAddr.sin_family = AF_INET;                      // IPv4 address family
	serverAddr.sin_port = htons(PORT);                    // Local port
	serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);       // Any incoming interface

	// Bind to the local address
	ret = bind(sockfd, (struct sockaddr*)&serverAddr, sizeof(serverAddr));
	if(ret < 0){
		printf("Error in binding.\n");
		perror("bind");
		exit(1);
	}
	printf("Bind to port %d\n", 4444);

	// Mark the socket so it will listen for incoming connections
	if(listen(sockfd, 10) == 0){                 //Maximum outstanding connection requests is 10
		printf("Listening....\n");
	}
	else {
		printf("Error in listening.\n");
		perror("listen");
	}

	while(1){   // Wait for clients to connect
		
		newSocket = accept(sockfd, (struct sockaddr*)&newAddr, &addr_size);
		if(newSocket < 0){
			exit(1);
		}

		// newSocket is connected to client!
		char newName[INET_ADDRSTRLEN]; // String to contain client address
		if (inet_ntop(AF_INET, &newAddr.sin_addr.s_addr, newName, sizeof(newName)) != NULL)
			printf("Handling client %s/ %d\n", newName, ntohs(newAddr.sin_port));
		else
			printf("Unable to get client address");

		//Using fork to handle concurrecy
		if ((childpid = fork()) < 0 ) 
			printf("Fork failed!");
		else if ((childpid = fork()) == 0 ) {
			close(sockfd); //child close parent socket
			while(1) {
				char buffer[BUF_SIZE]={0};
				int ret=1;
				int count=0;
				while(NULL == strchr(buffer,'\n') && ret > 0 && count < sizeof(buffer)) {
					ret=recv(newSocket, &buffer[count], sizeof(buffer)-count, 0);
					count+=ret;
				}
				if( ret <= 0 ) {
					if( ret < 0 ) {
						printf("Problem with recv: %d\n", ret);
					}
					break;
				}
				printf("recv: %s\n", buffer);
				strtok(buffer,"\n\r"); /* throw away any \r\n */
				if( ret> 0 ) {
					int i=0;
					char reply[20][BUF_SIZE]={0};
					char *p = strtok(buffer, ",");
					while (p!=NULL) {
						strncpy(reply[i],p,BUF_SIZE);
						strcat(reply[i],",");
						p = strtok(NULL, ","); //stroke received string into tokens
						i++;
					}
					//Merge sort
					mergesort(reply,20);
					for(int jj=0; jj<i; jj++) {
						printf("%s ", reply[jj]);
						ret=send(newSocket, reply[jj], strlen(reply[jj]), 0);
					}	
					ret=send(newSocket, "\n", 1, 0);
					printf("\n");
				}
			}
			exit(0); //Child process terminates
		}
		printf("with child process %d\n", childpid);
		close(newSocket); //Parent close child socket descriptor
		childProcCount++; //Increasement number of child processes
		while (childProcCount) { // Clean up all zombies
			childpid = waitpid((pid_t) - 1, NULL, WNOHANG); // Non-blocking wait
			if (childpid < 0) // waitpid() error?
				printf("waitpid() failed");
			else if (childpid == 0) // No zombie to wait on
				break;
			else
				childProcCount--; // Cleaned up after a child
		} 
	} 
	return 0;
}

int mergesort(char list[20][BUF_SIZE], int length) {  // First part
	mergesort_r(0, length, list);
	return 0;
}

int mergesort_r(int left, int right, char list[20][BUF_SIZE]) { // Overloaded portion

	if (right - left <= 1) {
        	return 0;
	}

	int left_start  = left;
	int left_end    = (left+right)/2;
	int right_start = left_end;
	int right_end   = right;

	mergesort_r( left_start, left_end, list);
	mergesort_r( right_start, right_end, list);
	merge(list, left_start, left_end, right_start, right_end);
}

int merge(char list[20][BUF_SIZE], int left_start, int left_end, int right_start, int right_end) {

	int left_length = left_end - left_start;
	int right_length = right_end - right_start;

	char left_half[left_length][BUF_SIZE];
	char right_half[right_length][BUF_SIZE];

	int r = 0;
	int l = 0;
	int i = 0;

	for (i = left_start; i < left_end; i++, l++) {
		strcpy(left_half[l], list[i]);
	}

	for (i = right_start; i < right_end; i++, r++) {
		strcpy(right_half[r], list[i]);
	}

	for ( i = left_start, r = 0, l = 0; l < left_length && r < right_length; i++) {
		if ( strcmp(left_half[l], right_half[r])<0 ) { 
			strcpy(list[i], left_half[l++]); 
		}
		else { 
		strcpy(list[i], right_half[r++]); }
		}

	for ( ; l < left_length; i++, l++) { 
		strcpy(list[i], left_half[l]); 
	}
	for ( ; r < right_length; i++, r++) { 
		strcpy(list[i], right_half[r]); 
	}
    	return 0;
}
