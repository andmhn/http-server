#include "utils.h"
#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>

int init(void);

extern char PORT[];
char SERVING_DIR[BUFSIZ] = {0};

void sigintHandler() {
    printf("Do You Want to Close Server?? (y/n):");
    if(getchar() == 'y')
    {
        log_msg("\nExiting...");
        exit(0);
    }
    else
        log_msg("Continuing...");
}

static void help_exit(int status)
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

static void ask_input()
{
    puts("You didn't provide arguments");
    puts("Please do now!");
    printf("Serving Folder (empty for current dir): ");

    uint i = 0, c;
    for(;i < sizeof(SERVING_DIR) && ((c = getchar()) != '\n'); i++)
    {
        SERVING_DIR[i] = c;
    }
    if(i == 0)
        SERVING_DIR[0] = '.';

    printf("Serving Port (empty for 8080): ");

    for(uint i = 0, c;i < 4 && ((c = getchar()) != '\n'); i++)
    {
        if(i == 0)  // now we know user has given inputs
            memset(PORT, 0, 4);
        PORT[i] = c;
    }
}

// parse the command line arguments
static void parse_arguments(int argc, char*argv[])
{
    char* dir = NULL;
    int c;
    while((c = getopt(argc, argv, "d:p:h")) != -1)
    {
        switch (c)
        {
            case 'd':
                dir = optarg;
                break;
            case 'p':
                strncpy(PORT, optarg, 4);
                break;
            case 'h':
                help_exit(0);
                break;
            default:
                help_exit(1);
        }
    }

    if(dir == NULL)
    {
        // set dir from non option arg
        dir = argv[optind];
        optind++;
    }

    // if we still have args
    if(optind < argc)
        help_exit(1);

    if(dir == NULL)
        dir = getenv("SERVING_DIR");

    if(dir == NULL)
    {
        log_msg("[ERR]: SERVING_DIR not known");
        help_exit(1);
    }

    strcpy(SERVING_DIR, dir);
}

int main(int argc, char *argv[]) 
{
    if(argc == 1)
        ask_input();
    else
        parse_arguments(argc, argv);

    // start serving now
    if (is_dir(SERVING_DIR)) {
        if (!has_permission(SERVING_DIR)) {
            log_msg("Access Denied");
            exit(1);
        }

        char msg[BUFSIZ + 100] = {0};
        sprintf(msg, "Starting Server in: %s", SERVING_DIR);
        log_msg(msg);

        memset(msg, 0, BUFSIZ);
        sprintf(msg, "Local URL http://localhost:%s", PORT);
        log_msg(msg);
    } else {
        log_perr(SERVING_DIR);
        exit(1);
    }
    chdir(SERVING_DIR);

    signal(SIGINT, sigintHandler);

    init();
}
