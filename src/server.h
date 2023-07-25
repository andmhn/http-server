#include <sys/socket.h>

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
void process_req(const char *request_str, int client_fd);
int verify_filepath(const char *file_path);
char *make_filepath(const char *get_str);
int handle_text_file(int sock_fd, char *filename);
void send_file(int sock_fd, const char *f_name);
