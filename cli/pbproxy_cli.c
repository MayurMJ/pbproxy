#include <string.h>
#include <unistd.h>
#include "pbheader.h"
#include <stdlib.h>
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <arpa/inet.h>
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
	struct sockaddr_in address;
	int sock = 0;
	struct sockaddr_in serv_addr;
	char *hello = "Hello from client";
	char buffer[1024] = {0};
	if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        	printf("\n Socket creation error \n");
       		return -1;
	}

	memset(&serv_addr, '0', sizeof(serv_addr));

	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(args->port);

	if(inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr)<=0) {
		printf("\nInvalid address/ Address not supported \n");
		return -1;
    	}
  
	if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    	{
        	printf("\nConnection Failed \n");
		return -1;
	}
	send(sock , hello , strlen(hello) , 0 );
	printf("Hello message sent\n");
	read( sock , buffer, 1024);
	printf("%s\n",buffer );
	return 0;
}
