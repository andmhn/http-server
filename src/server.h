#include <sys/socket.h>

#define BACKLOG 10        // how many pending connections queue will hold
#define BUFFERSIZE 0x8000 // 32k buffer (adapt at will)

extern char SERVING_DIR[];
char header_ok[]  = "HTTP/1.0 200 OK\r\n\n";
char header_404[] = "HTTP/1.0 404 Not Found\r\n\n";
char header_403[] = "HTTP/1.0 403 Forbidden\r\n\n";
char header_400[] = "HTTP/1.0 400 Bad Request\r\n\n";
char header_501[] = "501 Not Implemented\r\n\n";

// Function declarations
int init(void);
void *get_in_addr(struct sockaddr *sa);
void accept_req(int sockfd);
void process_req(const char *request_str, int client_fd);
void handle_request_value(int sock_fd, char *filename);
void send_file(int sock_fd, const char *f_name);
