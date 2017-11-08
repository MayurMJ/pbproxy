#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include "pbheader.h"

char* decrypt(char *msg, int length) {
	char iv[AES_BLOCK_SIZE];
	for(int i = 0; i < AES_BLOCK_SIZE; i++) {
		iv[i] = *msg;
		msg++;
	}
        AES_KEY key;
        int bytes_to_encrypt = 0;
        char *encrypted_text = (char *) malloc(sizeof(char) * length + 1);
	memset(encrypted_text, 0, sizeof(char) * length + 1);
	
        char *ret = encrypted_text;
        ctr state;
        const char* enc_key = "1234567812345678";
        if (AES_set_encrypt_key(enc_key, 128, &key) < 0) {
                fprintf(stderr, "Could not set encryption key.");
                exit(1);
        }
        state.num = 0;
        memset(state.count, 0, AES_BLOCK_SIZE);
        memset(state.iv + 8, 0, 8);
        memcpy(state.iv, iv, 8);
	int nl = length - AES_BLOCK_SIZE;
        AES_ctr128_encrypt(msg, encrypted_text, nl, &key, state.iv, state.count, &state.num);
        return ret; 
}

int startServer(parsedArgs *args) {
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
		char *ret = decrypt(buffer, strlen(buffer));
		printf("%s\n", decrypt(buffer, strlen(buffer)));
		send(newSocket , hello , strlen(hello) , 0 );
		free (ret);
		//printf("Hello message sent\n");
	}
	return 0;
}
