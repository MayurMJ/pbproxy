#ifndef PBHEADER_H
#define PBHEADER_H


typedef struct pArgs {
	int port;
	char* file;
	int destPort;
}parsedArgs;

int startServer(parsedArgs*);
int startClient(parsedArgs*);
#endif
