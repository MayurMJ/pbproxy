#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include "pbheader.h"

char ivc_server[AES_BLOCK_SIZE] = {0};
char ivs_server[AES_BLOCK_SIZE] = {0};
void *sshCom(void *threadArgs) {
	tArgs *tA = (tArgs *) threadArgs;
	while(1) {
 		char buffer[8192] = {0};
		int n = read(tA->socket , buffer, 8192);
		if(n <= 0) break;
		if(n > 0) {
			
			char ret[8192] = {0};
        		AES_ctr128_encrypt(buffer, ret, n, &(tA->key), tA->state.iv, tA->state.count, &(tA->state.num));
        		send(tA->socket2 , ret, n , 0 );
		}
	}
}

int createSSHConnection(parsedArgs *args) {
	struct sockaddr_in address;
        int sock = 0;
        struct sockaddr_in serv_addr;
        if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
                fprintf(stderr, "\n Socket creation error \n");
                return -1;
        }

        memset(&serv_addr, '0', sizeof(serv_addr));

        serv_addr.sin_family = AF_INET;
        serv_addr.sin_port = htons(atoi(args->dest[1]));

        if(inet_pton(AF_INET, args->dest[0], &serv_addr.sin_addr)<=0) {
                fprintf(stderr, "\nInvalid address/ Address not supported \n");
                return -1;
        }

        if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
        {
                fprintf(stderr, "\nConnection Failed \n");
                return -1;
        }
	return sock;
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
		fprintf(stderr,"socket failed");
		exit(1);
	}
      
	if (setsockopt(serverFd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) {
		fprintf(stderr, "setsockopt");
		exit(1);
	}
	address.sin_family = AF_INET;
	address.sin_addr.s_addr = INADDR_ANY;
	address.sin_port = htons(args->port);
      
	if (bind(serverFd, (struct sockaddr *)&address, sizeof(address))<0) {
		fprintf(stderr, "bind failed");
		exit(1);
	}
	while(1) {
		if (listen(serverFd, 3) < 0) {
			fprintf(stderr, "listen");
			exit(1);
		}
		if ((newSocket = accept(serverFd, (struct sockaddr *)&address, (socklen_t*)&addrLen))<0) {
			fprintf(stderr,"accept");
			exit(1);
		}
		if(!RAND_bytes(ivs_server, AES_BLOCK_SIZE)) {
			fprintf(stderr, "Could not create random bytes.");
			exit(1);
		}
		int n = read( newSocket , ivc_server, 8192);
		send(newSocket, ivs_server, AES_BLOCK_SIZE , 0 );
		int sshSocket = createSSHConnection(args);	
		tArgs *ssht = (tArgs*) malloc(sizeof(tArgs));
		ssht->socket = newSocket;
		ssht->socket2 = sshSocket;
		AES_KEY key;
        	if (AES_set_encrypt_key(buff, 128, &key) < 0) {
                	fprintf(stderr, "Could not set encryption key.");
                	exit(1);
        	}
		ssht->key = key;
        	ssht->state.num = 0;
        	memset(ssht->state.count, 0, AES_BLOCK_SIZE);
       		memset(ssht->state.iv + 8, 0, 8);
        	memcpy(ssht->state.iv, ivc_server, 8);
		
		tArgs *pbt = (tArgs*) malloc(sizeof(tArgs));
		pbt->socket2 = newSocket;
		pbt->socket = sshSocket;
		AES_KEY key1;
        	if (AES_set_encrypt_key(buff, 128, &key1) < 0) {
                	fprintf(stderr, "Could not set encryption key.");
                	exit(1);
        	}
		pbt->key = key;
        	pbt->state.num = 0;
        	memset(pbt->state.count, 0, AES_BLOCK_SIZE);
       		memset(pbt->state.iv + 8, 0, 8);
        	memcpy(pbt->state.iv, ivs_server, 8);
		pthread_t tid, tid2;
		pthread_create(&tid, NULL, sshCom, (void*) ssht);	
		pthread_create(&tid2, NULL, sshCom, (void*) pbt);
	}
	return 0;
}
