#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include "pbheader.h"

//void readIn(void *sock) {
//	int i = (int) *sock;
//}


char *encrypt(char *msg, int length, const char* enc_key) {
	AES_KEY key;
	char iv[AES_BLOCK_SIZE];
	char *text = malloc(sizeof(char) * length + AES_BLOCK_SIZE);
	memset(text, 0, sizeof(char) * length + AES_BLOCK_SIZE);
	int bytes_to_encrypt = 0;
	char *encrypted_text = (char *) malloc(sizeof(char) * length);
	memset(encrypted_text, 0, sizeof(char) * length);
	char *ret = encrypted_text;
	ctr state;
	if(!RAND_bytes(iv, AES_BLOCK_SIZE)) {
		fprintf(stderr, "Could not create random bytes.");
		exit(1);
	}
	if (AES_set_encrypt_key(enc_key, 128, &key) < 0) {
		fprintf(stderr, "Could not set encryption key.");
		exit(1);
	}
	state.num = 0;
	memset(state.count, 0, AES_BLOCK_SIZE);
	memset(state.iv + 8, 0, 8);
 	memcpy(state.iv, iv, 8);
	AES_ctr128_encrypt(msg, encrypted_text, length, &key, state.iv, state.count, &state.num);
	strcat(text, iv);
	strcat(text, ret);
	free(ret);
	return text;
	
}

int startClient(parsedArgs *args) {
        FILE *fp;
        fp = fopen(args->file, "r");
        char buff[4096] = {0};
        if(fp)
                fgets(buff, 4096, (FILE*)fp);	
	struct sockaddr_in address;
	int sock = 0;
	struct sockaddr_in serv_addr;

	char buffer[1024] = {0};
	if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        	printf("\n Socket creation error \n");
       		return -1;
	}

	memset(&serv_addr, '0', sizeof(serv_addr));

	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(atoi(args->dest[1]));

	if(inet_pton(AF_INET, args->dest[0], &serv_addr.sin_addr)<=0) {
		printf("\nInvalid address/ Address not supported \n");
		return -1;
    	}
  
	if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    	{
        	printf("\nConnection Failed \n");
		return -1;
	}
	//pthread_t tid;
	//pthread_create(&tid, NULL, readIn, void(*) &sock);	
	char *hello = (char *) malloc(sizeof(char)* 1024); //"Hello from client";
	memset(hello, 0, 1024);
	read(STDIN_FILENO, hello, 1024);
	char *sendMsg = encrypt(hello, strlen(hello), buff);
	send(sock , sendMsg , strlen(sendMsg) , 0 );	
	//printf("%s\n", hello);
	read( sock , buffer, 1024);
	write(STDOUT_FILENO, buffer, strlen(buffer));
	//write(STDOUT_FILENO, buffer, strlen(buffer));
	//printf("%s\n",buffer );
	free(sendMsg);
	free(hello);
	return 0;
}
