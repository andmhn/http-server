#include <stdio.h>
#include <stdlib.h>
#include <string.h>

enum request { GET, HEAD, POST };

// contains the request and it's content
typedef struct HttpRequest {
    char *value;
    int method;
} HttpRequest;

int find_method(char *curr_line);
void fill_req_content(const char *req_str, HttpRequest *request);
int parse_req(const char *req, HttpRequest *request);
