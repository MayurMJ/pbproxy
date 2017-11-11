#ifndef PBHEADER_H
#define PBHEADER_H
#include <openssl/aes.h>
#include <openssl/rand.h>
#include <stdio.h>
#include <string.h>
#include <pthread.h>

typedef struct pArgs {
	int port;
	char* file;
	char* dest[2];
}parsedArgs;

typedef struct ctrstruct {
	int num;
	char count[AES_BLOCK_SIZE];
	char iv[AES_BLOCK_SIZE];
}ctr;


typedef struct threadArgs {
	int socket;
	int socket2;
	ctr state;
	AES_KEY key;
	char buff[100];
}tArgs;

int startServer(parsedArgs*);
int startClient(parsedArgs*);
#endif
