#include "utils.h"
#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int init(void);

char SERVING_DIR[BUFSIZ];

void sigintHandler() { exit(0); }

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Usage: http-server <folder to serve>");
        exit(1);
    }
    strncpy(SERVING_DIR, argv[1], strlen(argv[1]));

    if (is_dir(SERVING_DIR)) {
        if (!has_permission(SERVING_DIR)) {
            printf("Access Denied\n");
            exit(1);
        }
        printf("Serving in: %s\n\n", SERVING_DIR);
    } else {
        perror(SERVING_DIR);
        exit(1);
    }
    chdir(SERVING_DIR);

    signal(SIGINT, sigintHandler);
    init();
}
