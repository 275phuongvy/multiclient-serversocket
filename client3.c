#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>

#define PORT 4444
#define BUF_SIZE 1024

int main(int argc, char *argv[])
{
	if (argc < 2 || argc > 3)      // Test for correct number of arguments
		printf("Parameter(): <Server Address>");	
	char *servIP = argv[1];        // arg: server IP address (dotted quad)

	int clientSocket, ret;
	struct sockaddr_in serverAddr; // Server address
	char buffer[BUF_SIZE];

	// Create a reliable, stream socket using TCP
	clientSocket = socket(AF_INET, SOCK_STREAM, 0);
	if(clientSocket < 0){
		printf("Error in creating socket.\n");
		exit(1);
	}
	printf("Client Socket is created.\n");

	// Construct the server address structure
	memset(&serverAddr, '\0', sizeof(serverAddr)); // Zero out structure
	serverAddr.sin_family = AF_INET;               // IPv4 address family
	serverAddr.sin_port = htons(PORT);             // Server port
	// Convert address
	int rtnVal = inet_pton(AF_INET, servIP, &serverAddr.sin_addr.s_addr);
	if (rtnVal == 0)
		printf("inet_pton() failed: invalid address string");
	else if (rtnVal < 0)
		printf("inet_pton() failed");

	// Establish the connection to the sorted server
	ret = connect(clientSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr));
	if(ret < 0){
		printf("Error in connection.\n");
		exit(1);
	}
	printf("Connected to Server.\n");
	do {
	//Read arrays from input file
		FILE *fptr;
		fptr = fopen("client3.inputdat", "r"); //open input file to read
		if (fptr != NULL) {
			while(fgets(buffer, sizeof(buffer), fptr)) {   //read line by line of the input file
				char reply[BUF_SIZE]={0};
				char repList[20][BUF_SIZE]={0};
				int count=0;
				int ret=1;
				int k=0;
				// Send array to the server
				send(clientSocket, buffer, strlen(buffer), 0);
				char *array[20];
				char *pt = strtok(buffer, ",");
				while (pt!=NULL) {
					array[k++] = pt;
					pt = strtok(NULL, ",");
				}
				printf("\n");
				printf("Unsorted array 3: ");	
				for (k=0; k<20; k++) {
					printf("%s ", array[k]);
				}
	    	
				// Receive the sorted arrays back from the server
				while(NULL == strchr(reply,'\n') && ret > 0 && count < sizeof(reply)) {
					ret=recv(clientSocket, &reply[count], sizeof(reply) - count,0);
					count+=ret;
				}
				if( ret < 0 ) {
					printf("Error in receiving data.\n");
				}
				else {
					int i=0;
					strtok(reply,"\n\r");
					for(char *p=strtok(reply,",");p!=NULL; p=strtok(NULL,",")) {
						strncpy(repList[i],p,BUF_SIZE);
						i++;
					}
					printf("Sorted array 3: ");
					for (int jj=0; jj<i; jj++) {
						printf("%s ", repList[jj]);
					}	
				}
				printf("\n");
			}
			fclose(fptr);
		}
    		else {
			printf("File does not exist");
			perror("fopen");
			exit(1);
	}
	printf("\n");
	printf("Do you want to exit the the client (yes/no)?: \n");
	scanf("%s", &buffer[0]);
	} while (strcmp(buffer, "yes") != 0);
	close(clientSocket);
	printf("Disconnected from server.\n");
	exit(1);
	return 0;
}
