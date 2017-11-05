#include<string.h>
#include <unistd.h>
#include"pbheader.h"
#include<stdlib.h>
#include <stdio.h>

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
}
