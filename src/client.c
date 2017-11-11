#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include "pbheader.h"

char iv[AES_BLOCK_SIZE] = {0};
char ivs[AES_BLOCK_SIZE]= {0};
char *encrypt(char *msg, int length, const char* enc_key, char ret[8192]);
void *readIn(void *tA) {
	tArgs *threadargs = (tArgs *) tA;
	while(1) {	
		char msg[8192] = {0};
		int n = read(STDIN_FILENO, msg, 8192);
		if(n > 0) {
			char ret[8192] = {0};
			AES_ctr128_encrypt(msg, ret, n, &(threadargs->key), threadargs->state.iv, threadargs->state.count, &(threadargs->state.num));
			send(threadargs->socket , ret , n , 0 );
		}
	}
}

void *writeOut(void *threadargs) {	
	tArgs *tA = (tArgs *) threadargs;
	while(1) {	
		char msg[8192] = {0};
		int n = read( tA->socket , msg, 8192);
		if(n <= 0) break;
		if(n > 0) {
			char ret[8192] = {0};
			AES_ctr128_encrypt(msg, ret, n, &(tA->key), tA->state.iv, tA->state.count, &(tA->state.num));	
			write(STDOUT_FILENO, ret, n);
		}
	}
}


char *encrypt(char *msg, int length, const char* enc_key, char ret[8192]) {
	AES_KEY key;
	ctr state;
	if (AES_set_encrypt_key(enc_key, 128, &key) < 0) {
		fprintf(stderr, "Could not set encryption key.");
		exit(1);
	}
	state.num = 0;
	memset(state.count, 0, AES_BLOCK_SIZE);
	memset(state.iv + 8, 0, 8);
 	memcpy(state.iv, iv, 8);
	AES_ctr128_encrypt(msg, ret, length, &key, state.iv, state.count, &state.num);
	return ret;
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
	if(!RAND_bytes(iv, AES_BLOCK_SIZE)) {
		fprintf(stderr, "Could not create random bytes.");
		exit(1);
	}
	//printf("\n Connecting");
	send(sock, iv , AES_BLOCK_SIZE , 0 );
	int n = read( sock , ivs, 8192);	

	/*char *msg = "Hello Hi There";
	char *ret = encrypt(msg, strlen(msg), buff);
        char decrypt[100] = { 0 };
	send(sock, ret, strlen(ret) , 0 );
	read( sock , decrypt, 8192);
	printf("\nmsg orig: %s", msg);	
	printf("\ndecrypt: %s", decrypt);*/
	tArgs *ta = (tArgs*) malloc(sizeof(tArgs));
	ta->socket = sock;
	AES_KEY key;
        if (AES_set_encrypt_key(buff, 128, &key) < 0) {
                fprintf(stderr, "Could not set encryption key.");
                exit(1);
        }
        ta->state.num = 0;
        memset(ta->state.count, 0, AES_BLOCK_SIZE);
        memset(ta->state.iv + 8, 0, 8);
        memcpy(ta->state.iv, iv, 8);
	ta->key = key;
	
	AES_KEY key1;
        if (AES_set_encrypt_key(buff, 128, &key1) < 0) {
                fprintf(stderr, "Could not set encryption key.");
                exit(1);
        }
	tArgs *ta1 = (tArgs*) malloc(sizeof(tArgs));
	ta1->socket = sock;
        ta1->state.num = 0;
        memset(ta1->state.count, 0, AES_BLOCK_SIZE);
        memset(ta1->state.iv + 8, 0, 8);
        memcpy(ta1->state.iv, ivs, 8);
	ta1->key = key1;

	pthread_t tid, tid2;
	pthread_create(&tid, NULL, readIn, (void*) ta);	
	pthread_create(&tid2, NULL, writeOut, (void*) ta1);
	pthread_join(tid, NULL);
     	pthread_join(tid2, NULL);
	
	//printf("%s\n", hello);
	//write(STDOUT_FILENO, buffer, strlen(buffer));
	//printf("%s\n",buffer );
	//free(sendMsg);
	//free(hello);
	return 0;
}
