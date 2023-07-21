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

#include "utils.h"

#define PORT "3490" // the port users will be connecting to

#define BACKLOG 10 // how many pending connections queue will hold

int accept_req(int sockfd);

// get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa) {
    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in *)sa)->sin_addr);
    }

    return &(((struct sockaddr_in6 *)sa)->sin6_addr);
}

int init(void) {
    int sockfd, new_fd; // listen on sock_fd, new connection on new_fd
    struct addrinfo hints, *servinfo, *p;
    struct sockaddr_storage client_addr; // connector's address information
    socklen_t sin_size;
    struct sigaction sa;
    int yes = 1;
    char s[INET6_ADDRSTRLEN];
    int rv;

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

    printf("server: waiting for connections...\n");

	accept_req(sockfd);

    return 0;
}

int accept_req(int sockfd){
	int err;
	size_t f_size = 3000;
	char * response_str = read_file("res/more-references.html", &err, &f_size);
	if(err != FILE_OK){
		fprintf(stderr, "Couldn't read file");
		exit(1);
	}

	char http_header[4000] = "HTTP/1.1 200 OK\r\n\n";
	strncat(http_header, response_str, f_size);


	struct sockaddr_storage client_addr;
	socklen_t sin_size;
	int new_fd;
    char s[INET6_ADDRSTRLEN];

    while (1) { // main accept() loop
        sin_size = sizeof client_addr;
        new_fd = accept(sockfd, (struct sockaddr *)&client_addr, &sin_size);
        if (new_fd == -1) {
            perror("accept");
            continue;
        }

        inet_ntop(client_addr.ss_family,
                  get_in_addr((struct sockaddr *)&client_addr), s, sizeof s);
        printf("server: got connection from %s\n", s);

        if (send(new_fd, http_header, strlen(http_header), 0) == -1)
            perror("send");
        close(new_fd);
    }

}
