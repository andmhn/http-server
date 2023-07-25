#include "server.h"
#include "parser.h"
#include "utils.h"

#include <arpa/inet.h>
#include <errno.h>
#include <netdb.h>
#include <netinet/in.h>
#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

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

// read and send content to sock_fd
void send_file(int sock_fd, const char *f_name) {
    FILE *fp1 = fopen(f_name, "rb");

    if (fp1 == NULL) {
        perror(f_name);
        exit(1);
    }

    for (;;) // loop for ever
    {
        char buffer[BUFFERSIZE];
        size_t bytesread = fread(buffer, sizeof(char), sizeof buffer, fp1);

        // bytesread contains the number of bytes actually read
        if (bytesread == 0) {
            // no bytes read => end of file
            break;
        }

        // send here
        if (send(sock_fd, buffer, BUFFERSIZE, 0) == -1)
            perror("send");
    }

    fclose(fp1);
}

// thinking on file path
int handle_text_file(int sock_fd, char *filename) {
    if (ends_with(filename, "/")) {
        strncat(filename, "index.html", 11);
    }
    // TODO check if path is directory

    send_file(sock_fd, filename);

    return 0;
}

char *make_filepath(const char *filename) {
    char *file_to_read = (char *)malloc(BUFSIZ * sizeof(char));
    memset(file_to_read, 0, BUFSIZ);
    strncpy(file_to_read, SERVING_DIR, strlen(SERVING_DIR)); // making the path
    strncat(file_to_read, filename, strlen(filename));

    return file_to_read;
}

int verify_filepath(const char *file_path) {
    FILE *temp = fopen(file_path, "rb");
    if (temp == NULL) {
        return -1;
    }
    fclose(temp);
    return 0;
}

// processes http requests and responds them
void process_req(const char *request_str, int client_fd) {
    // Parse String
    HttpRequest incoming_request;
    if (parse_req(request_str, &incoming_request) == -1) {
        fprintf(stderr, "Got Invalid request\n");
        return;
    }

    // TODO implement head and post

    char *filepath = make_filepath(incoming_request.value);

    // check if file exist
    if (verify_filepath(filepath) == -1) {
        perror(filepath);
        if (send(client_fd, header_404, strlen(header_404), 0) == -1)
            perror("send");
        goto exit;
    }

    // sending 200 OK first
    if (send(client_fd, header_ok, strlen(header_ok), 0) == -1)
        perror("send");

    // handle binary file first
    if (is_binary(filepath)) {
        send_file(client_fd, filepath);
        goto exit;
    }

    // check file loading error
    if (handle_text_file(client_fd, filepath) == -1) {
        goto exit;
    }

exit:
    printf("\n");
    fflush(stdout);
    free(filepath);
}
