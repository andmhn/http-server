enum request { GET = 0, HEAD, POST };

// contains the request and it's content
typedef struct HttpRequest {
    char *value;
    int method;
} HttpRequest;

int find_method(char *curr_line);
void fill_req_content(const char *req_str, HttpRequest *request);
int parse_req(const char *req, HttpRequest *request);
char *parse_encoded_url(const char *url);
