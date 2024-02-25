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

char PORT[] = "8080";  // the port users will be connecting to
bool IS_RUNNING = 1;

void send_folder_content(const char *folder_name, int client_fd);

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
        char msg[BUFSIZ] = {0};
        sprintf(msg, "getaddrinfo: %s\n", gai_strerror(rv));
        log_msg(msg);
        return 1;
    }

    // loop through all the results and bind to the first we can
    for (p = servinfo; p != NULL; p = p->ai_next) {
        if ((sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) ==
            -1) {
            log_perr("server: socket");
            continue;
        }
        if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) ==
            -1) {
            log_perr("setsockopt");
            exit(1);
        }
        if (bind(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
            close(sockfd);
            log_perr("server: bind");
            continue;
        }

        break;
    }

    freeaddrinfo(servinfo); // all done with this structure

    if (p == NULL) {
        log_perr("server: failed to bind\n");
        exit(1);
    }

    if (listen(sockfd, BACKLOG) == -1) {
        log_perr("listen");
        exit(1);
    }

    accept_req(sockfd);
    close(sockfd);
    return 0;
}

void accept_req(int sockfd) {
    struct sockaddr_storage client_addr;
    socklen_t sin_size;
    int client_fd;
    char s[INET6_ADDRSTRLEN];

    while (1 && IS_RUNNING) { // main accept() loop
        char request_str[BUFSIZ];
        log_msg("\nserver: waiting for connections...");
        sin_size = sizeof client_addr;
        client_fd = accept(sockfd, (struct sockaddr *)&client_addr, &sin_size);
        if (client_fd == -1) {
            log_perr("accept");
            continue;
        }

        // getting ip address of client
        inet_ntop(client_addr.ss_family,
                  get_in_addr((struct sockaddr *)&client_addr), s, sizeof s);
        char msg[BUFSIZ] = {0};
        sprintf(msg, "server: got connection from %s", s);
        log_msg(msg);

        // recieving
        if (recv(client_fd, request_str, BUFSIZ, 0) == -1) {
            log_perr("recv");
            close(client_fd);
            break;
        }

        process_req(request_str, client_fd);

        close(client_fd);
    }
}

// read and send content to sock_fd
void send_file(int sock_fd, const char *f_name) {
    FILE *fp1 = fopen(f_name, "rb");

    if (fp1 == NULL) {
        log_perr(f_name);
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

        // send only read bytes
        if (send(sock_fd, buffer, bytesread, 0) == -1) {
            log_perr("send");
            break;
        }
    }

    fclose(fp1);
}

// thinking on file path
void handle_request_value(int sock_fd, char *file_path) {
    // if it's root directory
    if (strcmp(file_path, "./") == 0) {
        strncat(file_path, "index.html", 11);

        if (verify_filepath(file_path) == -1)
            send_folder_content("./", sock_fd);
        else
            send_file(sock_fd, file_path);
    }

    // check if path is directory
    else if (is_dir(file_path))
        send_folder_content(file_path, sock_fd);

    else
        send_file(sock_fd, file_path);
}

// processes http requests and responds them
void process_req(const char *request_str, int client_fd) {
    // Parse String
    HttpRequest incoming_request;
    if (parse_req(request_str, &incoming_request) == -1) {
        log_msg("[ERR]: Got Invalid request");
        return;
    }

    // send head
    if (incoming_request.method == HEAD) {
        if (send(client_fd, header_ok, strlen(header_ok), 0) == -1)
            log_perr("send");

        return;
    }

    // all other requests
    if (incoming_request.method > 1) {
        if (send(client_fd, header_501, strlen(header_501), 0) == -1)
            log_perr("send");
        return;
    }

    char filepath[strlen(incoming_request.value) + 2];
    sprintf(filepath, ".%s", incoming_request.value); // prepending with .
    char *parsed_url = parse_encoded_url(filepath);

    // check if file exist
    if (verify_filepath(parsed_url) == -1) {
        log_perr(parsed_url);
        if (send(client_fd, header_404, strlen(header_404), 0) == -1)
            log_perr("send");
        goto exit;
    }

    // check permission to view content
    if (verify_filepath(parsed_url) && !has_permission(parsed_url)) {
        if (send(client_fd, header_403, strlen(header_403), 0) == -1)
            log_perr("send");
        goto exit;
    }

    // forbid . and .. in url
    if (str_starts_with(incoming_request.value, "./") ||
        str_starts_with(incoming_request.value, "../")) {
        if (send(client_fd, header_400, strlen(header_400), 0) == -1)
            log_perr("send");
        goto exit;
    }

    // sending 200 OK first
    if (send(client_fd, header_ok, strlen(header_ok), 0) == -1)
        log_perr("send");

    // serving content
    handle_request_value(client_fd, parsed_url);

exit:
//    printf("\n");
    fflush(stdout);
    free(parsed_url);
    free(incoming_request.value);
}
