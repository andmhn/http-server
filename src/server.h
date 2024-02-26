#pragma once
#include <sys/socket.h>

#define BACKLOG 10        // how many pending connections queue will hold
#define BUFFERSIZE 0x8000 // 32k buffer (adapt at will)

extern char SERVING_DIR[];

extern char header_ok[];
extern char header_404[];
extern char header_403[];
extern char header_400[];
extern char header_501[];

// Function declarations
int init(void);
void *get_in_addr(struct sockaddr *sa);
void accept_req(int sockfd);
void process_req(const char *request_str, int client_fd);
void handle_request_value(int sock_fd, char *filename);
void send_file(int sock_fd, const char *f_name);
