#include "utils.h"
#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>

// <dir> -p port

int init(void);

char *SERVING_DIR;

void sigintHandler() { exit(0); }

char* PORT = "8080";  // the port users will be connecting to


void help_exit(int status)
{
    fprintf(stderr,
			"Usage: http-server [OPTIONS] [folder to serve]\n"
		   "\n"
		   "-d \t Directory To Serve\n"
		   " \t else set envioroment variable SERVING_DIR\n\n"
		   "-p \t set port of server\n"
		   " \t It is default to 8080\n\n"
		   "-h \t Display this help message\n"
			);
	exit(status);
}

int main(int argc, char *argv[]) 
{	
	SERVING_DIR = getenv("SERVING_DIR");

	// parse the command line arguments
	if (argc < 2)
		help_exit(1);

	int c;
	while((c = getopt(argc, argv, "d:p:h")) != -1)
	{
		switch (c)
		{
			case 'd':
				SERVING_DIR = optarg;
				break;
			case 'p':
				PORT = optarg;
				break;
			case 'h':
				help_exit(0);
				break;
			default:
				help_exit(1);
		}
	}

	if(SERVING_DIR == NULL)
	{
		// set dir from non option arg
		SERVING_DIR = argv[optind];
		optind++;
	}

	// if we still have args
	if(optind < argc)
		help_exit(1);
	
	if(SERVING_DIR == NULL)
	{
		fprintf(stderr, "[ERROR]: SERVING_DIR not known\n\n");
		help_exit(1);
	}

	// start serving now
    if (is_dir(SERVING_DIR)) {
        if (!has_permission(SERVING_DIR)) {
            printf("Access Denied\n");
            exit(1);
        }
        printf("Serving in: %s\n", SERVING_DIR);
		printf("Local URL http://127.0.0.1:%s\n\n", PORT);
    } else {
        perror(SERVING_DIR);
        exit(1);
    }
    chdir(SERVING_DIR);

    signal(SIGINT, sigintHandler);
    init();
}
