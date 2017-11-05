#include <string.h>
#include <unistd.h>
#include "pbheader.h"
#include <stdlib.h>
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>

void parseArgs(parsedArgs *args, char **argv, int n) {
	/*for(int i = 1; i < n; i++) {
		if(strcmp(argv[i], "-i") == 0 ) {		
			args->interface = string(argv[i+1]);
			i++;
		}
		else if(strcmp(argv[i], "-r") == 0 ) {
			args->file = string(argv[i+1]);
			i++;
		}
		else if(strcmp(argv[i], "-s") == 0 ) {
			args->str = string(argv[i+1]);
			i++;
		}
		else {
			if(argv[i] != NULL) {			
				args->exp += " "; 
				args->exp += string(argv[i]);
			}
		}
	}*/
	int c, index;
	while ((c = getopt (n, argv, "l:k:")) != -1)
	switch (c)
	{
      		case 'l':
			args->port = atoi(optarg);
			break;
		case 'k':
			args->file = optarg;
			break;
		default:
			exit(0);
	}
	for (index = optind; index < n; index++) {
		args->destPort = atoi(argv[index]);
	}
}

int main(int argc, char** argv) {
	parsedArgs *args = (parsedArgs*)malloc(sizeof(parsedArgs));
	parseArgs(args, argv, argc);
	printf("\n Port: %d, File: %s, Dest: %d", args->port, args->file, args->destPort);
	int serverFd, newSocket;
	struct sockaddr_in address;
	int opt = 1;
	int addrLen = sizeof(address);
 	char buffer[1024] = {0};
	char *hello = "Hello from server";
      
	if ((serverFd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
		perror("socket failed");
		exit(EXIT_FAILURE);
	}
      
	if (setsockopt(serverFd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) {
		perror("setsockopt");
		exit(EXIT_FAILURE);
	}
	address.sin_family = AF_INET;
	address.sin_addr.s_addr = INADDR_ANY;
	address.sin_port = htons(args->port);
      
	if (bind(serverFd, (struct sockaddr *)&address, sizeof(address))<0) {
		perror("bind failed");
		exit(EXIT_FAILURE);
	}
	while(1) {
		if (listen(serverFd, 3) < 0) {
			perror("listen");
			exit(EXIT_FAILURE);
		}
		if ((newSocket = accept(serverFd, (struct sockaddr *)&address, (socklen_t*)&addrLen))<0) {
			perror("accept");
			exit(EXIT_FAILURE);
		}
		read(newSocket , buffer, 1024);
		printf("%s\n",buffer );
		send(newSocket , hello , strlen(hello) , 0 );
		printf("Hello message sent\n");
	}
	return 0;
}
