#include <arpa/inet.h>
#include <errno.h>
#include <netdb.h>
#include <netinet/in.h>
#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include "parser.h"
#include "utils.h"

#define PORT "3490"       // the port users will be connecting to
#define BACKLOG 10        // how many pending connections queue will hold
#define BUFFERSIZE 0x8000 // 32k buffer (adapt at will)

extern char SERVING_DIR[];
char header_ok[] = "HTTP/1.0 200 OK\r\n\n";
char header_404[] = "HTTP/1.0 404 Not Found\r\n\n";

// Function declarations
int init(void);
void *get_in_addr(struct sockaddr *sa);
int accept_req(int sockfd);
void process_req(char *request_str, int client_fd);
int verify_filepath(const char *file_path);
char *get_filepath(char *get_str);
int handle_text_file(int sock_fd, char *filename);
void send_file(int sock_fd, const char *f_name);

/*
 * Function Implementations
 * */

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
        strncat(filename, "index.html", 10);
    }
    send_file(sock_fd, filename);

    // TODO check if path is directory
    return 0;
}

char *get_filepath(char *get_str) {
    char *filename = get_file_str(get_str);
    char *file_to_read = malloc(BUFSIZ * sizeof(char));
    memset(file_to_read, 0, BUFSIZ);
    strncpy(file_to_read, SERVING_DIR, strlen(SERVING_DIR)); // making the path
    strncat(file_to_read, filename, strlen(filename));

    free(filename);
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
void process_req(char *request_str, int client_fd) {
    // Parse String
    char *get_req_str = parse_get_req(request_str);
    printf("%s\n", get_req_str);

    char *filepath = get_filepath(get_req_str);

    // check if file exist
    if (verify_filepath(filepath)) {
        if (send(client_fd, header_404, strlen(header_404), 0) == -1)
            perror("send");
        goto exit;
    }

    // handle binary file first
    if (is_binary(filepath)) {
        if (send(client_fd, header_ok, strlen(header_ok), 0) == -1)
            perror("send");
        send_file(client_fd, filepath);
        goto exit;
    }

    // sending 200 OK first
    if (send(client_fd, header_ok, strlen(header_ok), 0) == -1)
        perror("send");

    // check file loading error
    if (handle_text_file(client_fd, filepath) == -1) {
        goto exit;
    }

exit:
    free(get_req_str);

    printf("\n");
    fflush(stdout);
}
