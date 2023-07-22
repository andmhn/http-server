/*
** server.c -- a stream socket server demo
*/

#include <arpa/inet.h>
#include <errno.h>
#include <netdb.h>
#include <netinet/in.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include "parser.h"
#include "utils.h"

#define PORT "3490" // the port users will be connecting to

#define BACKLOG 10 // how many pending connections queue will hold

extern char SERVING_DIR[];

#define SIZ 50000

int accept_req(int sockfd);
void process_req(char *request_str, int client_fd);

// get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa) {
    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in *)sa)->sin_addr);
    }

    return &(((struct sockaddr_in6 *)sa)->sin6_addr);
}

int init(void) {
    int sockfd, yes = 1, rv;
    struct addrinfo hints, *servinfo, *p;

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE; // use my IP

    if ((rv = getaddrinfo(NULL, PORT, &hints, &servinfo)) != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        return 1;
    }

    // loop through all the results and bind to the first we can
    for (p = servinfo; p != NULL; p = p->ai_next) {
        if ((sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) ==
            -1) {
            perror("server: socket");
            continue;
        }
        if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) ==
            -1) {
            perror("setsockopt");
            exit(1);
        }
        if (bind(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
            close(sockfd);
            perror("server: bind");
            continue;
        }

        break;
    }

    freeaddrinfo(servinfo); // all done with this structure

    if (p == NULL) {
        fprintf(stderr, "server: failed to bind\n");
        exit(1);
    }

    if (listen(sockfd, BACKLOG) == -1) {
        perror("listen");
        exit(1);
    }

    accept_req(sockfd);
    return 0;
}

int accept_req(int sockfd) {
    struct sockaddr_storage client_addr;
    socklen_t sin_size;
    int client_fd;
    char s[INET6_ADDRSTRLEN];

    while (1) { // main accept() loop
        char request_str[BUFSIZ];
        printf("server: waiting for connections...\n");
        sin_size = sizeof client_addr;
        client_fd = accept(sockfd, (struct sockaddr *)&client_addr, &sin_size);
        if (client_fd == -1) {
            perror("accept");
            continue;
        }

        // getting ip address of client
        inet_ntop(client_addr.ss_family,
                  get_in_addr((struct sockaddr *)&client_addr), s, sizeof s);
        printf("server: got connection from %s\n", s);

        // recieving
        recv(client_fd, request_str, BUFSIZ, 0);

		process_req(request_str, client_fd);

        close(client_fd);
    }
}


// processes http requests and responds them
void process_req(char *request_str, int client_fd) {
    char header_ok[SIZ + 500] = "HTTP/1.0 200 OK\r\n\n";

    // Parse String
    char *get_req_str = parse_get_req(request_str);
    printf("%s\n", get_req_str);

	char * filename = get_file_str(get_req_str);	

	char * file_content;
	int err; size_t size = SIZ;
	char file_to_read[BUFSIZ];
	memset(file_to_read, 0, BUFSIZ);

	strncpy(file_to_read, SERVING_DIR, 3);
	strncat(file_to_read, filename, strlen(filename));

	// TODO differentiate b/w html and binary

	if(!strcmp(filename, "/")){
		strncat(file_to_read, "index.html", 10);
		file_content = read_file(file_to_read , &err, &size);
	}
	else {
		file_content = read_file(file_to_read, &err, &size);
	}

	if(err != FILE_OK){
	    printf("FILE NOT EXIST %s\n", file_to_read);
    	if (send(client_fd, header_ok, strlen(header_ok), 0) == -1)
        	perror("send");
		printf("\n\n");
	    return;
	}

	strncat(header_ok, file_content, size);

    if (send(client_fd, header_ok, strlen(header_ok), 0) == -1)
        perror("send");

	free(filename);
	free(file_content);
	free(get_req_str);

	printf("\n\n");
	fflush(stdout);
}
