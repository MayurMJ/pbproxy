#include <string.h>
#include <unistd.h>
#include "pbheader.h"
#include <stdlib.h>
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>

void parseArgs(parsedArgs *args, char **argv, int n) {
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
	args->port = -1;
	parseArgs(args, argv, argc);
	printf("\n Port: %d, File: %s, Dest: %d", args->port, args->file, args->destPort);
	if(args->port != -1) {
		startServer(args);
	}
	else {
		startClient(args);
	}
}
