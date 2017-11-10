#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include "pbheader.h"

void *sshCom(void *threadArgs) {
	tArgs *tA = (tArgs *) threadArgs;
	while(1) {
 		char buffer[8192] = {0};
		int n = read(tA->socket , buffer, 8192);
		if(n > 0) {
        		send(tA->socket2 , buffer, n , 0 );
			//printf("\n Buffer: %s", buffer);
		}
	}
}

void *pbCom(void *threadArgs) {
	tArgs *tA = (tArgs *) threadArgs;
	while(1) {
 		char buffer[8192] = {0};
		int n = read(tA->socket2 , buffer, 8192);
		if(n > 0) {
        		send(tA->socket2 , buffer, n , 0 );	
			//printf("\n Buffer2: %s", buffer);
		}
	}
}

int createSSHConnection(parsedArgs *args) {
	struct sockaddr_in address;
        int sock = 0;
        struct sockaddr_in serv_addr;
	//char *buffer = (char *) malloc (sizeof(char) * 1024);
	//memset(buffer, 0, 1024);
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
        //send(sock , sendMsg , strlen(sendMsg) , 0 );
        //read( sock , buffer, 1024);
	//write(STDOUT_FILENO, buffer, strlen(buffer));
        //free(sendMsg);
	return sock;
}

char* decrypt(char *msg, int length, const char *enc_key) {
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
	FILE *fp;
	fp = fopen(args->file, "r");
	char buff[4096] = {0};
	if(fp)
		fgets(buff, 4096, (FILE*)fp);
	int serverFd, newSocket;
	struct sockaddr_in address;
	int opt = 1;
	int addrLen = sizeof(address);
      
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
	if (listen(serverFd, 3) < 0) {
		perror("listen");
		exit(EXIT_FAILURE);
	}
	if ((newSocket = accept(serverFd, (struct sockaddr *)&address, (socklen_t*)&addrLen))<0) {
		perror("accept");
		exit(EXIT_FAILURE);
	}
	int sshSocket = createSSHConnection(args);	
	tArgs *ssht = (tArgs*) malloc(sizeof(tArgs));
	ssht->socket = newSocket;
	ssht->socket2 = sshSocket;
	ssht->key = buff;
	tArgs *pbt = (tArgs*) malloc(sizeof(tArgs));
	pbt->socket = sshSocket;
	pbt->socket2 = newSocket;
	pbt->key = buff;
	pthread_t tid, tid2;
//	pthread_create(&tid, NULL, sshCom, (void*) ssht);	
	pthread_create(&tid2, NULL, pbCom, (void*) pbt);
//	pthread_join(tid, NULL);
     	pthread_join(tid2, NULL);
	/*while(1) {
 		char buffer[1024] = {0};
		read(newSocket , buffer, 1024);
		//printf("We are%s\n", buffer);
		//char *ret = decrypt(buffer, strlen(buffer), buff);
		//char *replyBack = forwardMsg(ret, strlen(ret), args);
		//printf("We are%s\n", decrypt(buffer, strlen(buffer), buff));
		//send(newSocket , replyBack, strlen(replyBack) , 0 );
		//send(newSocket , ret,  strlen(ret) , 0 );	
		send(newSocket , buffer,  strlen(buffer) , 0 );	
		//free (ret);
		//free (replyBack);
		//printf("Hello message sent\n");
	}*/
	return 0;
}
